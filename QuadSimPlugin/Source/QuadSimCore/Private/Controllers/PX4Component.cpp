#include "Controllers/PX4Component.h"
#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "Engine/World.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/PlatformProcess.h"

// MAVLink includes - use the correct path structure
#pragma warning(push)
#pragma warning(disable: 4005) // Disable macro redefinition warnings
#pragma warning(disable: 4996) // Disable deprecated function warnings

// Include the common dialect of MAVLink
#include "common/mavlink.h"

#pragma warning(pop)

DEFINE_LOG_CATEGORY_STATIC(LogPX4, Log, All);

// Static member definition
const double FPX4CommunicationThread::TARGET_INTERVAL = 1.0 / FPX4CommunicationThread::TARGET_FREQUENCY_HZ;

// ========================================
// Communication Thread Implementation
// ========================================

FPX4CommunicationThread::FPX4CommunicationThread(UPX4Component* InPX4Component)
    : PX4Component(InPX4Component)
    , Thread(nullptr)
    , bStopRequested(false)
{
}

FPX4CommunicationThread::~FPX4CommunicationThread()
{
    StopThread();
}

bool FPX4CommunicationThread::Init()
{
    UE_LOG(LogPX4, Warning, TEXT("PX4 Communication thread initialized"));
    return true;
}

uint32 FPX4CommunicationThread::Run()
{
	UE_LOG(LogPX4, Warning, TEXT("PX4 Communication thread started - running at %d Hz"), TARGET_FREQUENCY_HZ);
    
	double LastTime = FPlatformTime::Seconds();
	double LastHeartbeatTime = FPlatformTime::Seconds();
	int32 MessageCount = 0;
	const double HeartbeatInterval = 0.5; // 2Hz heartbeat
    
	// Use high precision timing
	const double TargetInterval = 1.0 / 250.0; // 4ms
    
	while (!bStopRequested)
	{
		double StartTime = FPlatformTime::Seconds();
        
		if (PX4Component && PX4Component->IsConnectedToPX4())
		{
			// Update state from main thread
			PX4Component->UpdateThreadSafeState();
            
			// Send HIL data
			PX4Component->SendHILDataFromThread();
            
			// Send heartbeat from thread too
			if (StartTime - LastHeartbeatTime >= HeartbeatInterval)
			{
				PX4Component->SendHeartbeat();
				LastHeartbeatTime = StartTime;
			}
            
			MessageCount++;
			if (MessageCount % 250 == 0) // Log every second
			{
				UE_LOG(LogPX4, VeryVerbose, TEXT("Thread: Sent %d messages, rate: %.1f Hz"), 
					   MessageCount, MessageCount / (StartTime - LastTime));
			}
		}
        
		// Precise sleep to maintain 250Hz
		double ElapsedTime = FPlatformTime::Seconds() - StartTime;
		double SleepTime = TargetInterval - ElapsedTime;
        
		if (SleepTime > 0)
		{
			FPlatformProcess::Sleep(SleepTime);
		}
		else if (MessageCount % 100 == 0)
		{
			UE_LOG(LogPX4, Warning, TEXT("Communication thread falling behind! Took %.2fms, target: %.2fms"), 
				   ElapsedTime * 1000.0, TargetInterval * 1000.0);
		}
	}
    
	return 0;
}

void FPX4CommunicationThread::Stop()
{
    bStopRequested = true;
}

void FPX4CommunicationThread::Exit()
{
    // Thread cleanup
}

void FPX4CommunicationThread::StartThread()
{
    if (!Thread && !bStopRequested)
    {
        bStopRequested = false;
    	Thread = FRunnableThread::Create(this, TEXT("PX4CommunicationThread"), 0, TPri_TimeCritical);    }
}

void FPX4CommunicationThread::StopThread()
{
    if (Thread)
    {
        bStopRequested = true;
        Thread->WaitForCompletion();
        delete Thread;
        Thread = nullptr;
    }
}

// ========================================
// PX4Component Implementation
// ========================================

UPX4Component::UPX4Component()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
    
    // CRITICAL: For lockstep mode, use 250Hz
    StateUpdateRate = 250.0f; // Must match PX4's expected rate
    HeartbeatRate = 2.0f;     // 2Hz heartbeat is sufficient
    
    // Initialize sockets to nullptr
    TCPListenSocket = nullptr;
    TCPClientSocket = nullptr;
    UDPSendSocket = nullptr;
    UDPRecvSocket = nullptr;
    
    // Threading setup
    CommunicationThread = nullptr;
    bThreadSafeDataValid = false;

	mavlink_system.sysid = SystemID;
	mavlink_system.compid = ComponentID;
}

void UPX4Component::BeginPlay()
{
    Super::BeginPlay();
	UE_LOG(LogPX4, Warning, TEXT("PX4Component BeginPlay - SystemID=%d, ComponentID=%d"), 
	   SystemID, ComponentID);
    UE_LOG(LogPX4, Warning, TEXT("PX4Component BeginPlay started"));
    
    // Try to find the QuadDroneController
    QuadController = FindQuadController();
    
    if (!QuadController)
    {
        UE_LOG(LogPX4, Warning, TEXT("QuadController not found in BeginPlay, will retry later"));
    }
    
    if (bUsePX4)
    {
        ConnectToPX4();
    }
}

void UPX4Component::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stop communication thread first
    if (CommunicationThread)
    {
        UE_LOG(LogPX4, Warning, TEXT("Stopping PX4 communication thread"));
        CommunicationThread->StopThread();
        delete CommunicationThread;
        CommunicationThread = nullptr;
    }
    
    DisconnectFromPX4();
    Super::EndPlay(EndPlayReason);
}

void UPX4Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bUsePX4) return;
    
    // Check for incoming TCP connections if we're listening
    if (bTCPListening && !bTCPConnected)
    {
        AcceptTCPConnection();
    }
    
    // Only proceed if we have an active connection
    if (!TCPClientSocket) 
    {
        return;
    }
    
    // Process incoming MAVLink data FIRST
    ProcessIncomingMAVLinkData();
    
    // Update timers
    HeartbeatTimer += DeltaTime;
    
    // Only update connection timeout if we're expecting heartbeats
    if (bTCPConnected && bConnectedToPX4)
    {
        ConnectionTimeoutTimer += DeltaTime;
    }
    
    // Send sensor data in lockstep mode
    if (bTCPConnected && bUseLockstep)
    {
        // Log every 50 frames
        if (LockstepCounter % 50 == 0)
        {
            UE_LOG(LogPX4, Warning, TEXT("Lockstep[%llu]: timestamp=%llu us, fps=%.1f"), 
                   LockstepCounter, LockstepCounter * 4000, 1.0f / DeltaTime);
        }
        
        UpdateCurrentState();
        
        // Send all HIL messages
        SendHILSensor();
        SendHILStateQuaternion();
        SendHILGPS();
        SendHILRCInputs();
        
        // CRITICAL: Force TCP to send data immediately
        // This is a platform-specific call, but it ensures data is sent
        if (TCPClientSocket)
        {
            // Force the socket to send any buffered data
            int32 BytesSent = 0;
            TCPClientSocket->Send(nullptr, 0, BytesSent); // Empty send to flush
        }
        
        LockstepCounter++;
    }
    
    // Send heartbeat
    if (bTCPConnected && HeartbeatTimer >= (1.0f / HeartbeatRate))
    {
        SendHeartbeat();
        HeartbeatTimer = 0.0f;
    }
    
    // Connection timeout - only check if we've received at least one heartbeat
    if (bConnectedToPX4 && ConnectionTimeoutTimer > 30.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection timeout after %.1fs"), ConnectionTimeoutTimer);
        bConnectedToPX4 = false;
        bTCPConnected = false;
        
        if (CommunicationThread)
        {
            CommunicationThread->StopThread();
            delete CommunicationThread;
            CommunicationThread = nullptr;
        }
    }
}
void UPX4Component::SetPX4Active(bool bActive)
{
    if (bActive && !bUsePX4)
    {
        bUsePX4 = true;
        ConnectToPX4();
        
        // Tell QuadController to use external control
        if (QuadController)
        {
            QuadController->SetUseExternalController(true);
        }
    }
    else if (!bActive && bUsePX4)
    {
        bUsePX4 = false;
        DisconnectFromPX4();
        
        // Tell QuadController to use internal control
        if (QuadController)
        {
            QuadController->SetUseExternalController(false);
        }
    }
}

void UPX4Component::ConnectToPX4()
{
    if (bConnectedToPX4 || bTCPListening) return;
    
    UE_LOG(LogPX4, Warning, TEXT("Starting TCP server on port %d for PX4 simulator"), PX4_Port);
    
    // Setup TCP server to listen for PX4 connections
    SetupTCPServer();
    
    if (bTCPListening)
    {
        ConnectionTimeoutTimer = 0.0f;
        UE_LOG(LogPX4, Warning, TEXT("TCP server listening on port %d - waiting for PX4 to connect..."), PX4_Port);
        UE_LOG(LogPX4, Warning, TEXT("Start PX4 with: make px4_sitl none_iris"));
    }

	if (bUseLockstep)
	{
		UE_LOG(LogPX4, Warning, TEXT("Using LOCKSTEP mode - PX4 will sync with Unreal frame rate"));
		// Don't start the communication thread
	}
	else
	{
		// Start thread for real-time mode
		if (!CommunicationThread)
		{
			CommunicationThread = new FPX4CommunicationThread(this);
			CommunicationThread->StartThread();
		}
	}
}

void UPX4Component::DisconnectFromPX4()
{
    if (!bConnectedToPX4 && !bTCPConnected && !bTCPListening) return;
    
    UE_LOG(LogPX4, Warning, TEXT("Disconnecting from PX4"));
    
    // Stop communication thread first
    if (CommunicationThread)
    {
        CommunicationThread->StopThread();
        delete CommunicationThread;
        CommunicationThread = nullptr;
    }
    
    CleanupSockets();
    bConnectedToPX4 = false;
    bTCPListening = false;
    bTCPConnected = false;
    bUDPReady = false;
}

bool UPX4Component::IsConnectedToPX4() const
{
    return bConnectedToPX4 && bTCPConnected;
}

void UPX4Component::UpdateThreadSafeState()
{
    if (!QuadController)
    {
        QuadController = FindQuadController();
    }
    
    FScopeLock Lock(&StateMutex);
    
    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        ThreadSafePosition = QuadPawn->GetActorLocation();
        ThreadSafeVelocity = QuadPawn->GetVelocity();
        ThreadSafeRotation = QuadPawn->GetActorRotation();
        
        if (QuadController)
        {
            ThreadSafeAngularVelocity = QuadController->GetLocalAngularRateDeg();
        }
        else
        {
            ThreadSafeAngularVelocity = FVector::ZeroVector;
        }
        
        bThreadSafeDataValid = true;
    }
    else
    {
        bThreadSafeDataValid = false;
    }
}

void UPX4Component::SendHILDataFromThread()
{
	FScopeLock Lock(&StateMutex);
    
	if (!bThreadSafeDataValid) 
	{
		CurrentPosition = FVector::ZeroVector;
		CurrentVelocity = FVector::ZeroVector;
		CurrentRotation = FRotator::ZeroRotator;
		CurrentAngularVelocity = FVector::ZeroVector;
	}
	else
	{
		CurrentPosition = ThreadSafePosition;
		CurrentVelocity = ThreadSafeVelocity;
		CurrentRotation = ThreadSafeRotation;
		CurrentAngularVelocity = ThreadSafeAngularVelocity;
	}
    
	// Send sensor data EVERY cycle at 250Hz
	SendHILSensor();
	SendHILStateQuaternion();
    
	// Send GPS at 50Hz (every 5 cycles instead of 10)
	static int32 GPSCounter = 0;
	if (++GPSCounter % 5 == 0)  // Was % 10
	{
		SendHILGPS();
	}
    
	// Send RC at 50Hz
	static int32 RCCounter = 0;
	if (++RCCounter % 5 == 0)  // Was % 10
	{
		SendHILRCInputs();
	}
}

void UPX4Component::SetupTCPServer()
{
    CleanupSockets(); // Clean up any existing sockets
    
    // Create TCP server socket
    TCPListenSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("PX4TCPListenSocket"), false);
    
    if (!TCPListenSocket)
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to create TCP listen socket"));
        return;
    }
    
    // Set socket options
    TCPListenSocket->SetNonBlocking(true);
    TCPListenSocket->SetReuseAddr(true);
    
    // Bind to local address on specified port
    TSharedRef<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    LocalAddr->SetAnyAddress(); // Listen on all interfaces
    LocalAddr->SetPort(PX4_Port);
    
    if (!TCPListenSocket->Bind(*LocalAddr))
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to bind TCP server socket to port %d - is it already in use?"), PX4_Port);
        CleanupSockets();
        return;
    }
    
    // Start listening for connections
    if (!TCPListenSocket->Listen(1)) // Only expect one connection from PX4
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to listen on TCP port %d"), PX4_Port);
        CleanupSockets();
        return;
    }
    
    bTCPListening = true;
    UE_LOG(LogPX4, Warning, TEXT("TCP server listening on port %d for PX4 connection"), PX4_Port);
}

void UPX4Component::AcceptTCPConnection()
{
    if (!TCPListenSocket || bTCPConnected) return;
    
    bool bHasPendingConnection = false;
    if (TCPListenSocket->HasPendingConnection(bHasPendingConnection) && bHasPendingConnection)
    {
        TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        TCPClientSocket = TCPListenSocket->Accept(*RemoteAddr, TEXT("PX4TCPClientSocket"));
        
        if (TCPClientSocket)
        {
            // CRITICAL: Set socket options for real-time communication
            TCPClientSocket->SetNonBlocking(true);
            TCPClientSocket->SetNoDelay(true); // Disable Nagle's algorithm
            
            // Platform-specific: Set TCP_NODELAY option
            int32 NoDelayValue = 1;
            TCPClientSocket->SetReuseAddr(true);
            TCPClientSocket->SetLinger(false, 0);
            
            // Set buffer sizes
            int32 SendBufferSize = 32768; // 32KB
            int32 RecvBufferSize = 32768; // 32KB
            
            int32 ActualSendSize = SendBufferSize;
            int32 ActualRecvSize = RecvBufferSize;
            
            TCPClientSocket->SetSendBufferSize(SendBufferSize, ActualSendSize);
            TCPClientSocket->SetReceiveBufferSize(RecvBufferSize, ActualRecvSize);
            
            bTCPConnected = true;
            ConnectionTimeoutTimer = 0.0f;
            
            PX4TCPAddress = RemoteAddr;
            
            UE_LOG(LogPX4, Warning, TEXT("PX4 connected from %s"), *RemoteAddr->ToString(true));
            UE_LOG(LogPX4, Warning, TEXT("TCP connection established with NoDelay=true"));
            UE_LOG(LogPX4, Warning, TEXT("TCP buffers: Send=%d, Recv=%d"), ActualSendSize, ActualRecvSize);
            
            if (bUseLockstep)
            {
                bConnectedToPX4 = true;
                UE_LOG(LogPX4, Warning, TEXT("Lockstep mode - starting sensor data transmission immediately"));
            }
            
            // Close the listen socket
            if (TCPListenSocket)
            {
                TCPListenSocket->Close();
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(TCPListenSocket);
                TCPListenSocket = nullptr;
                bTCPListening = false;
            }
        }
    }
}

void UPX4Component::SetupUDPSockets()
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    
    // Create UDP send socket
    UDPSendSocket = FUdpSocketBuilder(TEXT("PX4UDPSendSocket"))
        .AsNonBlocking()
        .AsReusable()
        .Build();
    
    if (!UDPSendSocket)
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to create UDP send socket"));
        return;
    }
    
    // Create UDP receive socket and bind to local port
    UDPRecvSocket = FUdpSocketBuilder(TEXT("PX4UDPRecvSocket"))
        .BoundToPort(ControlPortLocal)
        .AsNonBlocking()
        .AsReusable()
        .Build();
    
    if (!UDPRecvSocket)
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to create UDP receive socket on port %d"), ControlPortLocal);
        if (UDPSendSocket)
        {
            SocketSubsystem->DestroySocket(UDPSendSocket);
            UDPSendSocket = nullptr;
        }
        return;
    }
    
    // Create UDP addresses
    // Use the IP from PX4's TCP connection if we have it, otherwise use configured IP
    if (PX4TCPAddress.IsValid())
    {
        // Clone the TCP address and change the port for UDP
        PX4UDPAddress = SocketSubsystem->CreateInternetAddr();
        
        // Get the IP address from TCP connection
        uint32 IPValue = 0;
        PX4TCPAddress->GetIp(IPValue);
        PX4UDPAddress->SetIp(IPValue);
        PX4UDPAddress->SetPort(ControlPortRemote);
        
        UE_LOG(LogPX4, Warning, TEXT("Using PX4's actual IP from TCP connection for UDP: %s"), *PX4UDPAddress->ToString(false));
    }
    else
    {
        // Fallback to configured IP
        FIPv4Address IP;
        FIPv4Address::Parse(PX4_IP, IP);
        
        PX4UDPAddress = SocketSubsystem->CreateInternetAddr();
        PX4UDPAddress->SetIp(IP.Value);
        PX4UDPAddress->SetPort(ControlPortRemote);
    }
    
    LocalUDPAddress = SocketSubsystem->CreateInternetAddr();
    LocalUDPAddress->SetAnyAddress();
    LocalUDPAddress->SetPort(ControlPortLocal);
    
    bUDPReady = true;
    UE_LOG(LogPX4, Warning, TEXT("UDP sockets created - Local: %d, Remote: %d"), ControlPortLocal, ControlPortRemote);
}

void UPX4Component::CleanupSockets()
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    
    if (TCPListenSocket)
    {
        TCPListenSocket->Close();
        SocketSubsystem->DestroySocket(TCPListenSocket);
        TCPListenSocket = nullptr;
    }
    
    if (TCPClientSocket)
    {
        TCPClientSocket->Close();
        SocketSubsystem->DestroySocket(TCPClientSocket);
        TCPClientSocket = nullptr;
    }
    
    if (UDPSendSocket)
    {
        UDPSendSocket->Close();
        SocketSubsystem->DestroySocket(UDPSendSocket);
        UDPSendSocket = nullptr;
    }
    
    if (UDPRecvSocket)
    {
        UDPRecvSocket->Close();
        SocketSubsystem->DestroySocket(UDPRecvSocket);
        UDPRecvSocket = nullptr;
    }
    
    PX4TCPAddress.Reset();
    PX4UDPAddress.Reset();
    LocalUDPAddress.Reset();
    
    bTCPListening = false;
    bTCPConnected = false;
    bUDPReady = false;
}

void UPX4Component::SendMAVLinkMessage(const uint8* MessageBuffer, uint16 MessageLength)
{
    if (!TCPClientSocket || !bTCPConnected)
    {
        return;
    }
    
    // CRITICAL FIX: Don't cast the buffer as mavlink_message_t!
    // The buffer contains the serialized message, not the struct
    // MAVLink v1 format: [STX][LEN][SEQ][SYSID][COMPID][MSGID]...
    // MAVLink v2 format: [STX][LEN][FLAGS][SEQ][SYSID][COMPID][MSGID(3bytes)]...
    
    uint32 msgid = 0;
    if (MessageLength >= 6)
    {
        // For MAVLink v1, message ID is at byte 5
        // For MAVLink v2, it's more complex, but let's try v1 first
        if (MessageBuffer[0] == 0xFE) // MAVLink v1 start byte
        {
            msgid = MessageBuffer[5];
        }
        else if (MessageBuffer[0] == 0xFD) // MAVLink v2 start byte
        {
            // MAVLink v2 has 3-byte message ID at bytes 7,8,9
            msgid = MessageBuffer[7] | (MessageBuffer[8] << 8) | (MessageBuffer[9] << 16);
        }
    }
    
    // Log first few messages of each type
    static TMap<uint32, int32> MessageCounts;
    int32& Count = MessageCounts.FindOrAdd(msgid);
    Count++;
    
    if (Count <= 5 || (Count % 100 == 0))
    {
        UE_LOG(LogPX4, Warning, TEXT("Sending MAVLink msg ID=%d (0x%X), len=%d, count=%d"), 
               msgid, msgid, MessageLength, Count);
    }
    
    // Try to send
    int32 TotalBytesSent = 0;
    const uint8* DataToSend = MessageBuffer;
    int32 BytesRemaining = MessageLength;
    
    // Set socket to blocking mode temporarily
    TCPClientSocket->SetNonBlocking(false);
    
    while (BytesRemaining > 0)
    {
        int32 BytesSent = 0;
        bool bSent = TCPClientSocket->Send(DataToSend + TotalBytesSent, BytesRemaining, BytesSent);
        
        if (!bSent || BytesSent <= 0)
        {
            ConsecutiveSendFailures++;
            
            // Get socket error for debugging
            ESocketErrors LastError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
            UE_LOG(LogPX4, Error, TEXT("Failed to send MAVLink message ID=%d, error=%d, failures=%d"), 
                   msgid, (int32)LastError, ConsecutiveSendFailures);
            
            if (ConsecutiveSendFailures > 10)
            {
                UE_LOG(LogPX4, Error, TEXT("Too many send failures - closing connection"));
                bTCPConnected = false;
                bConnectedToPX4 = false;
            }
            break;
        }
        
        TotalBytesSent += BytesSent;
        BytesRemaining -= BytesSent;
        ConsecutiveSendFailures = 0;
    }
    
    // Restore non-blocking mode
    TCPClientSocket->SetNonBlocking(true);
}

// Switch to non-blocking recv with immediate data sending:
void UPX4Component::ProcessIncomingMAVLinkData()
{
	if (!TCPClientSocket || !bTCPConnected) return;
    
	// Check socket state first
	ESocketConnectionState SocketState = TCPClientSocket->GetConnectionState();
	static ESocketConnectionState LastSocketState = SCS_NotConnected;
    
	if (SocketState != LastSocketState)
	{
		UE_LOG(LogPX4, Warning, TEXT("TCP Socket state changed from %d to %d"), 
			   (int32)LastSocketState, (int32)SocketState);
		LastSocketState = SocketState;
	}
    
	if (SocketState != SCS_Connected)
	{
		UE_LOG(LogPX4, Error, TEXT("TCP socket in bad state: %d"), (int32)SocketState);
		bTCPConnected = false;
		return;
	}
    
	// Try multiple receive approaches
	uint8 TempBuffer[4096];
	int32 BytesRead = 0;
    
	// First, check if data is available
	uint32 PendingDataSize = 0;
	if (TCPClientSocket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
	{
		UE_LOG(LogPX4, VeryVerbose, TEXT("TCP has %d bytes pending"), PendingDataSize);
        
		// Read the data
		if (TCPClientSocket->Recv(TempBuffer, FMath::Min((uint32)sizeof(TempBuffer), PendingDataSize), BytesRead))
		{
			if (BytesRead > 0)
			{
				UE_LOG(LogPX4, VeryVerbose, TEXT("Received %d bytes from TCP"), BytesRead);
				ParseMAVLinkData(TempBuffer, BytesRead);
			}
		}
		else
		{
			ESocketErrors LastError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
			UE_LOG(LogPX4, Error, TEXT("TCP Recv failed with error: %d"), (int32)LastError);
		}
	}
}

void UPX4Component::ParseMAVLinkData(const uint8* Data, int32 DataLength)
{
    mavlink_message_t msg;
    mavlink_status_t status;
    
    UE_LOG(LogPX4, VeryVerbose, TEXT("ParseMAVLinkData: Processing %d bytes"), DataLength);
    
    for (int32 i = 0; i < DataLength; i++)
    {
        if (mavlink_parse_char(MAVLINK_COMM_0, Data[i], &msg, &status))
        {
            // Log every message type we receive
            UE_LOG(LogPX4, Warning, TEXT("Received MAVLink msg ID=%d from sys:%d comp:%d, seq=%d"), 
                   msg.msgid, msg.sysid, msg.compid, msg.seq);
            
            switch (msg.msgid)
            {
            case MAVLINK_MSG_ID_HEARTBEAT: // ID 0
                HandleHeartbeat(reinterpret_cast<const uint8*>(&msg), sizeof(msg));
                break;
                
            case MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS: // ID 93
                HandleActuatorOutputs(reinterpret_cast<const uint8*>(&msg), sizeof(msg));
                break;
                
            case MAVLINK_MSG_ID_COMMAND_LONG: // ID 76
                {
                    mavlink_command_long_t cmd;
                    mavlink_msg_command_long_decode(&msg, &cmd);
                    UE_LOG(LogPX4, Warning, TEXT("Received COMMAND_LONG: cmd=%d, param1=%f, param2=%f"), 
                           cmd.command, cmd.param1, cmd.param2);
                    
                    if (cmd.command == MAV_CMD_SET_MESSAGE_INTERVAL)
                    {
                        int32 MessageID = static_cast<int32>(cmd.param1);
                        float IntervalUs = cmd.param2;
                        float Frequency = IntervalUs > 0 ? 1000000.0f / IntervalUs : 0.0f;
                        UE_LOG(LogPX4, Warning, TEXT("PX4 requested message ID %d at %f Hz"), MessageID, Frequency);
                        
                        // Send ACK
                        mavlink_message_t ack_msg;
                        mavlink_command_ack_t ack;
                        ack.command = cmd.command;
                        ack.result = MAV_RESULT_ACCEPTED;
                        mavlink_msg_command_ack_encode(SystemID, ComponentID, &ack_msg, &ack);
                        
                        uint8 buffer[MAVLINK_MAX_PACKET_LEN];
                        uint16 len = mavlink_msg_to_send_buffer(buffer, &ack_msg);
                        SendMAVLinkMessage(buffer, len);
                    }
                }
                break;
                
            default:
                // Log unknown messages
                if (msg.msgid != MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS) // Avoid spam from actuator messages
                {
                    UE_LOG(LogPX4, VeryVerbose, TEXT("Received unhandled MAVLink msg ID=%d"), msg.msgid);
                }
                break;
            }
        }
    }
	
}

void UPX4Component::SendHeartbeat()
{
	mavlink_message_t msg;
	mavlink_heartbeat_t heartbeat;
    
	heartbeat.type = MAV_TYPE_GCS; // Simulator acts as GCS
	heartbeat.autopilot = MAV_AUTOPILOT_INVALID;
	heartbeat.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | 
						 MAV_MODE_FLAG_HIL_ENABLED |
						 MAV_MODE_FLAG_SAFETY_ARMED; // Add ARMED flag
	heartbeat.custom_mode = 0;
	heartbeat.system_status = MAV_STATE_ACTIVE;
	heartbeat.mavlink_version = MAVLINK_VERSION; // Should be 3 for MAVLink 2.0
    
	// Use MAVLink 2.0 if available
	mavlink_msg_heartbeat_encode_chan(SystemID, ComponentID, MAVLINK_COMM_0, &msg, &heartbeat);
    
	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
	UE_LOG(LogPX4, VeryVerbose, TEXT("Sending heartbeat: ver=%d, len=%d"), 
		   MAVLINK_VERSION, len);
    
	SendMAVLinkMessage(buffer, len);
}


void UPX4Component::SendHILStateQuaternion()
{
    mavlink_message_t msg;
    mavlink_hil_state_quaternion_t hil_state;
    
    memset(&hil_state, 0, sizeof(hil_state));
    
    // Use same timestamp as HIL_SENSOR for consistency
    uint64_t timestamp_us = LockstepCounter * 4000;
    hil_state.time_usec = timestamp_us;
    
    // Convert from Unreal to NED coordinates
    // Unreal: X=Forward, Y=Right, Z=Up (cm)
    // NED: X=North, Y=East, Z=Down (m)
    
    float pos_x_m = CurrentPosition.X / 100.0f;  // cm to m
    float pos_y_m = CurrentPosition.Y / 100.0f;
    float pos_z_m = -CurrentPosition.Z / 100.0f; // Flip Z for NED
    
    float vel_x_ms = CurrentVelocity.X / 100.0f;
    float vel_y_ms = CurrentVelocity.Y / 100.0f;
    float vel_z_ms = -CurrentVelocity.Z / 100.0f;
    
    // Convert rotation to quaternion
    FQuat UnrealQuat = CurrentRotation.Quaternion();
    
    // Unreal to NED quaternion conversion
    // This might need adjustment based on your coordinate system
    hil_state.attitude_quaternion[0] = UnrealQuat.W;  // w
    hil_state.attitude_quaternion[1] = UnrealQuat.X;  // x
    hil_state.attitude_quaternion[2] = UnrealQuat.Y;  // y
    hil_state.attitude_quaternion[3] = -UnrealQuat.Z; // z (flip for NED)
    
    // Angular velocities (rad/s)
    hil_state.rollspeed = FMath::DegreesToRadians(CurrentAngularVelocity.X);
    hil_state.pitchspeed = FMath::DegreesToRadians(CurrentAngularVelocity.Y);
    hil_state.yawspeed = FMath::DegreesToRadians(-CurrentAngularVelocity.Z);
    
    // GPS position (using fixed location for now)
    hil_state.lat = 473977372; // Zurich latitude * 1e7
    hil_state.lon = 85455939;  // Zurich longitude * 1e7
    hil_state.alt = (int32_t)((500.0f - pos_z_m) * 1000.0f); // Altitude in mm, 500m above sea level
    
    // Velocities in cm/s
    hil_state.vx = (int16_t)(vel_x_ms * 100.0f);
    hil_state.vy = (int16_t)(vel_y_ms * 100.0f);
    hil_state.vz = (int16_t)(vel_z_ms * 100.0f);
    
    // Ground speed
    float ground_speed = FMath::Sqrt(vel_x_ms * vel_x_ms + vel_y_ms * vel_y_ms);
    hil_state.ind_airspeed = (uint16_t)(ground_speed * 100.0f); // cm/s
    hil_state.true_airspeed = hil_state.ind_airspeed;
    
    // Accelerations (use gravity-compensated values)
    hil_state.xacc = 0;
    hil_state.yacc = 0;
    hil_state.zacc = 980; // 1g in cm/s^2
    
    // Encode and send
    mavlink_msg_hil_state_quaternion_encode(SystemID, ComponentID, &msg, &hil_state);
    
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    SendMAVLinkMessage(buffer, len);
}
void UPX4Component::SendHILSensor()
{
    mavlink_message_t msg;
    mavlink_hil_sensor_t hil_sensor;
    
    // Zero out the entire structure
    memset(&hil_sensor, 0, sizeof(hil_sensor));
    
    // CRITICAL: For lockstep, timestamp must advance by exactly 4000us per step
    uint64_t timestamp_us = LockstepCounter * 4000;
    hil_sensor.time_usec = timestamp_us;
    
    // Get drone state - make sure we have valid data
    if (!QuadController)
    {
        QuadController = FindQuadController();
        if (!QuadController)
        {
            UE_LOG(LogPX4, Warning, TEXT("No QuadController found, using default sensor values"));
        }
    }
    
    // Basic sensor data (drone at rest initially)
    FQuat DroneQuat = FQuat(CurrentRotation);
    FVector GravityWorld(0, 0, -9.81f);
    FVector GravityBody = DroneQuat.UnrotateVector(GravityWorld);
    
    // Accelerometer (m/s^2) - includes gravity
    hil_sensor.xacc = GravityBody.X;
    hil_sensor.yacc = GravityBody.Y;
    hil_sensor.zacc = GravityBody.Z;
    
    // Gyroscope (rad/s)
    hil_sensor.xgyro = FMath::DegreesToRadians(CurrentAngularVelocity.X);
    hil_sensor.ygyro = FMath::DegreesToRadians(CurrentAngularVelocity.Y);
    hil_sensor.zgyro = FMath::DegreesToRadians(-CurrentAngularVelocity.Z);
    
    // Magnetometer (Gauss) - normalized earth field
    FVector MagWorld(0.21523f, 0.0f, 0.43045f); // Realistic earth field values
    FVector MagBody = DroneQuat.UnrotateVector(MagWorld);
    hil_sensor.xmag = MagBody.X;
    hil_sensor.ymag = MagBody.Y;
    hil_sensor.zmag = MagBody.Z;
    
    // Barometer - sea level pressure
    float AltitudeMeters = -CurrentPosition.Z / 100.0f; // Convert cm to m
    float SeaLevelPressure = 101325.0f; // Pa
    float Temperature = 15.0f; // Celsius
    
    // Proper atmospheric model
    float Pressure = SeaLevelPressure * FMath::Pow(1.0f - 0.0065f * AltitudeMeters / (Temperature + 273.15f), 5.255f);
    
    hil_sensor.abs_pressure = Pressure / 100.0f; // Convert Pa to mbar
    hil_sensor.diff_pressure = 0.0f; // No airspeed
    hil_sensor.pressure_alt = AltitudeMeters;
    hil_sensor.temperature = Temperature - (AltitudeMeters * 0.0065f); // Temperature lapse
    
    // CRITICAL: Set ALL required fields
    hil_sensor.fields_updated = 
        (1 << 0) |  // xacc
        (1 << 1) |  // yacc
        (1 << 2) |  // zacc
        (1 << 3) |  // xgyro
        (1 << 4) |  // ygyro
        (1 << 5) |  // zgyro
        (1 << 6) |  // xmag
        (1 << 7) |  // ymag
        (1 << 8) |  // zmag
        (1 << 9) |  // abs_pressure
        (1 << 10) | // diff_pressure
        (1 << 11) | // pressure_alt
        (1 << 12);  // temperature
    
    // Set lockstep flag
    if (bUseLockstep)
    {
        hil_sensor.fields_updated |= (uint32)(1 << 31); // Set bit 31 for lockstep
    }
    
    hil_sensor.id = 0; // Sensor instance ID
    
    // Encode the message
    uint16 msg_len = mavlink_msg_hil_sensor_encode(SystemID, ComponentID, &msg, &hil_sensor);
    
    // Create buffer and serialize
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    // Debug logging
    static int32 DbgCounter = 0;
    if (++DbgCounter % 50 == 0)
    {
        UE_LOG(LogPX4, Warning, TEXT("HIL_SENSOR[%d]: time=%llu us, acc=[%.2f,%.2f,%.2f], gyro=[%.2f,%.2f,%.2f], mag=[%.2f,%.2f,%.2f], pres=%.1f"), 
               DbgCounter, hil_sensor.time_usec, 
               hil_sensor.xacc, hil_sensor.yacc, hil_sensor.zacc,
               hil_sensor.xgyro, hil_sensor.ygyro, hil_sensor.zgyro,
               hil_sensor.xmag, hil_sensor.ymag, hil_sensor.zmag,
               hil_sensor.abs_pressure);
    }
    
    SendMAVLinkMessage(buffer, len);
}


void UPX4Component::SendHILGPS()
{
	mavlink_message_t msg;
	mavlink_hil_gps_t hil_gps;
    
	memset(&hil_gps, 0, sizeof(hil_gps));
    
	// Same timestamp as other HIL messages
	uint64_t timestamp_us = LockstepCounter * 4000;
	hil_gps.time_usec = timestamp_us;
    
	// GPS position
	hil_gps.lat = 473977372; // Zurich * 1e7
	hil_gps.lon = 85455939;
    
	float alt_m = -CurrentPosition.Z / 100.0f; // Convert to meters
	hil_gps.alt = (int32_t)((500.0f + alt_m) * 1000); // mm above sea level
    
	// GPS accuracy
	hil_gps.eph = 100; // HDOP * 100
	hil_gps.epv = 100; // VDOP * 100
    
	// Velocities in cm/s
	hil_gps.vn = (int16_t)(CurrentVelocity.X); // Already in cm/s
	hil_gps.ve = (int16_t)(CurrentVelocity.Y);
	hil_gps.vd = (int16_t)(-CurrentVelocity.Z); // NED frame
    
	// Ground speed and course
	float ground_speed_cms = FVector2D(CurrentVelocity.X, CurrentVelocity.Y).Size();
	hil_gps.vel = (uint16_t)ground_speed_cms;
	hil_gps.cog = (uint16_t)(FMath::Atan2(CurrentVelocity.Y, CurrentVelocity.X) * 18000.0f / PI); // cdeg
    
	hil_gps.fix_type = 3; // 3D fix
	hil_gps.satellites_visible = 12;
    
	// Additional GPS fields for realism
	hil_gps.id = 0;
	hil_gps.yaw = 0; // Not available
    
	// Encode and send
	mavlink_msg_hil_gps_encode(SystemID, ComponentID, &msg, &hil_gps);
    
	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
	SendMAVLinkMessage(buffer, len);
}
void UPX4Component::SendHILRCInputs()
{
	mavlink_message_t msg;
	mavlink_hil_rc_inputs_raw_t hil_rc;
    
	memset(&hil_rc, 0, sizeof(hil_rc));
    
	// Same timestamp
	uint64_t timestamp_us = LockstepCounter * 4000;
	hil_rc.time_usec = timestamp_us;
    
	// RC channels (1000-2000 range, 1500 = center)
	hil_rc.chan1_raw = 1500; // Roll
	hil_rc.chan2_raw = 1500; // Pitch
	hil_rc.chan3_raw = 1000; // Throttle (min for safety)
	hil_rc.chan4_raw = 1500; // Yaw
	hil_rc.chan5_raw = 1800; // Mode switch (position mode)
	hil_rc.chan6_raw = 1000; // Aux
	hil_rc.chan7_raw = 1000;
	hil_rc.chan8_raw = 1000;
	hil_rc.chan9_raw = 1000;
	hil_rc.chan10_raw = 1000;
	hil_rc.chan11_raw = 1000;
	hil_rc.chan12_raw = 1000;
    
	hil_rc.rssi = 255; // Max signal
    
	// Encode and send
	mavlink_msg_hil_rc_inputs_raw_encode(SystemID, ComponentID, &msg, &hil_rc);
    
	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
	SendMAVLinkMessage(buffer, len);
}

void UPX4Component::SendBasicHILData()
{
    // This method is kept for compatibility but is now handled by SendHILDataFromThread
    SendHILDataFromThread();
}

void UPX4Component::HandleActuatorOutputs(const uint8* MessageBuffer, uint16 MessageLength)
{
    if (!bConnectedToPX4 || !bTCPConnected)
    {
        return;
    }
    mavlink_message_t* msg = (mavlink_message_t*)MessageBuffer;
    mavlink_hil_actuator_controls_t actuator_controls;
    mavlink_msg_hil_actuator_controls_decode(msg, &actuator_controls);
    
    // Convert actuator controls to motor commands
    TArray<float> MotorCommands;
    MotorCommands.SetNum(4);
    
    // PX4 sends normalized values (-1 to 1), convert to (0 to 1) for thrust
    for (int32 i = 0; i < 4 && i < 16; i++)
    {
        // Clamp and normalize from [-1,1] to [0,1]
        float NormalizedValue = FMath::Clamp((actuator_controls.controls[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
        MotorCommands[i] = NormalizedValue;
    }
    
    // Apply motor commands to QuadController
    if (QuadController)
    {
        QuadController->ApplyMotorCommands(MotorCommands);
    }
    
    static int32 ActuatorCount = 0;
    if (++ActuatorCount % 100 == 0) // Log every 100th message
    {
        UE_LOG(LogPX4, Log, TEXT("Received motor commands #%d: M1=%.3f, M2=%.3f, M3=%.3f, M4=%.3f"), 
               ActuatorCount, MotorCommands[0], MotorCommands[1], MotorCommands[2], MotorCommands[3]);
    }

}

void UPX4Component::HandleHeartbeat(const uint8* MessageBuffer, uint16 MessageLength)
{
	mavlink_message_t* msg = (mavlink_message_t*)MessageBuffer;
	mavlink_heartbeat_t heartbeat;
	mavlink_msg_heartbeat_decode(msg, &heartbeat);
    
	// CRITICAL: Reset connection timeout when we receive heartbeat
	ConnectionTimeoutTimer = 0.0f;
	LastHeartbeatTime = GetWorld()->GetTimeSeconds();
    
	UE_LOG(LogPX4, Warning, TEXT("Got PX4 Heartbeat! Type=%d, Autopilot=%d, BaseMode=0x%X, SystemStatus=%d"), 
		   heartbeat.type, heartbeat.autopilot, heartbeat.base_mode, heartbeat.system_status);
    
	// Update target system/component IDs from PX4
	TargetSystem = msg->sysid;
	TargetComponent = msg->compid;
    
	// Mark as connected if this is the first heartbeat
	if (!bConnectedToPX4)
	{
		bConnectedToPX4 = true;
		UE_LOG(LogPX4, Warning, TEXT("PX4 connection fully established - target sys=%d, comp=%d"), 
			   TargetSystem, TargetComponent);
	}
}

UQuadDroneController* UPX4Component::FindQuadController()
{
    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        if (QuadPawn->QuadController)
        {
            return QuadPawn->QuadController;
        }
    }
    return nullptr;
}

void UPX4Component::UpdateConnectionStatus()
{
    if (bConnectedToPX4 && ConnectionTimeoutTimer > ConnectionTimeout)
    {
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection timeout - disconnecting"));
        bConnectedToPX4 = false;
    }
}

void UPX4Component::ConvertUnrealToPX4Coordinates(const FVector& UnrealPos, const FVector& UnrealVel, 
                                                 const FRotator& UnrealRot, const FVector& UnrealAngVel,
                                                 float& OutX, float& OutY, float& OutZ,
                                                 float& OutVx, float& OutVy, float& OutVz,
                                                 float& OutQ0, float& OutQ1, float& OutQ2, float& OutQ3,
                                                 float& OutRollRate, float& OutPitchRate, float& OutYawRate)
{
    // Convert from Unreal (FLU: Forward-Left-Up) to PX4/NED (North-East-Down)
    // Unreal: X=Forward, Y=Right, Z=Up
    // PX4/NED: X=North, Y=East, Z=Down
    
    // Position: Convert cm to meters and flip coordinates
    OutX = UnrealPos.X / 100.0f;  // Forward -> North
    OutY = UnrealPos.Y / 100.0f;  // Right -> East  
    OutZ = -UnrealPos.Z / 100.0f; // Up -> Down (flip sign)
    
    // Velocity: Convert cm/s to m/s and flip coordinates
    OutVx = UnrealVel.X / 100.0f;  // Forward -> North
    OutVy = UnrealVel.Y / 100.0f;  // Right -> East
    OutVz = -UnrealVel.Z / 100.0f; // Up -> Down (flip sign)
    
    // Attitude: Convert rotation to quaternion with coordinate system conversion
    FQuat UnrealQuat = FQuat(UnrealRot);
    OutQ0 = UnrealQuat.W;
    OutQ1 = UnrealQuat.X;
    OutQ2 = UnrealQuat.Y; 
    OutQ3 = -UnrealQuat.Z; // Flip Z for coordinate conversion
    
    // Angular rates: Convert and flip as needed
    OutRollRate = UnrealAngVel.X;   // Roll rate (same axis)
    OutPitchRate = UnrealAngVel.Y;  // Pitch rate (same axis)
    OutYawRate = -UnrealAngVel.Z;   // Yaw rate (flip for coordinate conversion)
}

uint64 UPX4Component::GetSynchronizedTimestamp()
{
	FScopeLock Lock(&TimestampMutex);
    
	// Use a base timestamp and increment it consistently
	if (BaseTimestamp == 0)
	{
		BaseTimestamp = FPlatformTime::Cycles64();
	}
    
	// Calculate elapsed time since base
	uint64 CurrentCycles = FPlatformTime::Cycles64();
	double ElapsedSeconds = FPlatformTime::ToSeconds64(CurrentCycles - BaseTimestamp);
    
	// Return microseconds since start
	return static_cast<uint64>(ElapsedSeconds * 1000000.0);
}

void UPX4Component::UpdateCurrentState()
{
	if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
	{
		CurrentPosition = QuadPawn->GetActorLocation();
		CurrentVelocity = QuadPawn->GetVelocity();
		CurrentRotation = QuadPawn->GetActorRotation();
        
		if (!QuadController)
		{
			QuadController = FindQuadController();
		}
        
		if (QuadController)
		{
			CurrentAngularVelocity = QuadController->GetLocalAngularRateDeg();
		}
		else
		{
			CurrentAngularVelocity = FVector::ZeroVector;
		}
	}
}

void UPX4Component::SendMinimalTestSensor()
{
	mavlink_message_t msg;
	mavlink_hil_sensor_t hil_sensor;
    
	// Zero out the entire structure
	memset(&hil_sensor, 0, sizeof(hil_sensor));
    
	// Set minimal required fields
	hil_sensor.time_usec = LockstepCounter * 4000;
    
	// Basic IMU data (drone at rest)
	hil_sensor.xacc = 0.0f;
	hil_sensor.yacc = 0.0f; 
	hil_sensor.zacc = -9.81f; // Gravity
    
	hil_sensor.xgyro = 0.0f;
	hil_sensor.ygyro = 0.0f;
	hil_sensor.zgyro = 0.0f;
    
	// Basic magnetometer (earth field)
	hil_sensor.xmag = 0.2f;
	hil_sensor.ymag = 0.0f;
	hil_sensor.zmag = 0.4f;
    
	// Basic barometer (sea level)
	hil_sensor.abs_pressure = 1013.25f; // mbar at sea level
	hil_sensor.pressure_alt = 0.0f;
	hil_sensor.temperature = 20.0f;
    
	// CRITICAL: Set ALL the required fields in fields_updated
	hil_sensor.fields_updated = 0x1FFF; // All fields valid (bits 0-12)
    
	if (bUseLockstep)
	{
		hil_sensor.fields_updated |= (1 << 31); // Set lockstep flag
	}
    
	hil_sensor.id = 0;
    
	// Encode and send
	mavlink_msg_hil_sensor_encode_chan(SystemID, ComponentID, MAVLINK_COMM_0, &msg, &hil_sensor);
    
	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
	static int32 TestCounter = 0;
	if (++TestCounter % 50 == 0)
	{
		UE_LOG(LogPX4, Warning, TEXT("Sending test HIL_SENSOR: time=%llu, fields=0x%X, len=%d"), 
			   hil_sensor.time_usec, hil_sensor.fields_updated, len);
	}
    
	SendMAVLinkMessage(buffer, len);
}

