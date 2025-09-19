#include "Controllers/PX4Component.h"
#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "Engine/World.h"
#include "Sensors/BaroSensor.h"
#include "Sensors/GPSSensor.h"
#include "Sensors/IMUSensor.h"
#include "Sensors/MagSensor.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/PlatformProcess.h"

// MAVLink includes - use the correct path structure
#pragma warning(push)
#pragma warning(disable: 4005) // Disable macro redefinition warnings
#pragma warning(disable: 4996) // Disable deprecated function warnings

// Include the common dialect of MAVLink
#include "CoordinateTransform.h"
#include "Sensors/SensorManagerComponent.h"
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

	// Precise timing for the 250Hz loop
	double LastTime = FPlatformTime::Seconds();
	const double TargetInterval = 1.0 / static_cast<double>(TARGET_FREQUENCY_HZ); // 4ms

	// CRITICAL DEBUG: Track thread performance
	static int32 ThreadCycleCount = 0;
	static int32 MissedCycles = 0;
	static double TotalRunTime = 0.0;

	while (!bStopRequested)
	{
		double StartTime = FPlatformTime::Seconds();
		ThreadCycleCount++;

		if (PX4Component && PX4Component->IsConnectedToPX4())
		{
			// Update the drone's state from the main game thread
			PX4Component->UpdateThreadSafeState();

			// Handle the simulation step (sends HIL data, heartbeats, etc.)
			PX4Component->ThreadSimulationStep();

			// Process any data received from PX4
			PX4Component->ProcessIncomingMAVLinkData();
		}

		// Precise sleep to maintain the target frequency (e.g., 250Hz)
		double ElapsedTime = FPlatformTime::Seconds() - StartTime;
		double SleepTime = TargetInterval - ElapsedTime;
		TotalRunTime += ElapsedTime;

		if (SleepTime > 0)
		{
			FPlatformProcess::Sleep(static_cast<float>(SleepTime));
		}
		else if (PX4Component && PX4Component->IsConnectedToPX4())
		{
			// Count missed cycles for debugging
			MissedCycles++;
			UE_LOG(LogPX4, Warning, TEXT("Communication thread fell behind schedule by %.2f ms (total missed: %d)"),
				   -SleepTime * 1000.0, MissedCycles);
		}

		// Log thread performance every 10 seconds
		if (ThreadCycleCount % (TARGET_FREQUENCY_HZ * 10) == 0)
		{
			double AvgRunTime = TotalRunTime / ThreadCycleCount * 1000.0; // ms
			UE_LOG(LogPX4, Warning, TEXT("Thread performance: %d cycles, %.2fms avg, %d missed"),
				   ThreadCycleCount, AvgRunTime, MissedCycles);
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
        // Create thread with highest priority and increased stack size for real-time performance
        Thread = FRunnableThread::Create(this, TEXT("PX4CommunicationThread"), 1024 * 1024, TPri_TimeCritical);
        
        if (Thread)
        {
            UE_LOG(LogPX4, Warning, TEXT("PX4 communication thread created with TimeCritical priority"));
        }
    }
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

	// Use editor property for lockstep mode
	bUseLockstep = bUseLockstepMode;
	LockstepCounter = 1;
}

void UPX4Component::BeginPlay()
{
    Super::BeginPlay();
    
    // Sync lockstep mode from editor property
    bUseLockstep = bUseLockstepMode;
    
	UE_LOG(LogPX4, Warning, TEXT("PX4Component BeginPlay - SystemID=%d, ComponentID=%d, Lockstep=%s"), 
	   SystemID, ComponentID, bUseLockstep ? TEXT("Enabled") : TEXT("Disabled"));
    
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
    
	// Only handle TCP connection acceptance in Tick
	if (bTCPListening && !bTCPConnected)
	{
		AcceptTCPConnection();
	}
    
	// Process queued motor commands from the communication thread
	if (bConnectedToPX4 && QuadController)
	{
		FMotorCommand Command;
		while (PendingMotorCommands.Dequeue(Command))
		{
			// Now we're safely on the game thread
			QuadController->ApplyMotorCommands(Command.Commands);
			if (!PendingMotorCommands.IsEmpty())
			{
				UE_LOG(LogPX4, Warning, TEXT("Motor command queue depth: %d"), 
					   PendingMotorCommands.IsEmpty() ? 0 : 1);
			}
		}
	}
}

// In PX4Component.cpp - SimulationUpdate
void UPX4Component::SimulationUpdate(float FixedDeltaTime)
{
	if (!bTCPConnected || !bUseLockstep) return;
    
	// Update state from drone
	UpdateCurrentState();
    
	// Calculate how many sensor updates we need to send
	const float SensorUpdateInterval = 0.004f; // 250Hz = 4ms
	int32 UpdatesNeeded = FMath::RoundToInt(FixedDeltaTime / SensorUpdateInterval);
	UpdatesNeeded = FMath::Max(1, UpdatesNeeded);
    
	for (int32 i = 0; i < UpdatesNeeded; i++)
	{
		// Increment counter for each sensor update
		SimulationStepCounter++;
		LockstepCounter++; // Each step advances lockstep counter
        
		// Send sensor data
		SendHILSensor();
		SendHILStateQuaternion();
        
		// Send GPS and RC at lower rates
		if (SimulationStepCounter % 5 == 0) // 50Hz
		{
			SendHILGPS();
			SendHILRCInputs();
		}
	}
    
	// Send heartbeat occasionally
	static float HeartbeatAccumulator = 0.0f;
	HeartbeatAccumulator += FixedDeltaTime;
	if (HeartbeatAccumulator >= 0.5f) // 2Hz
	{
		SendHeartbeat();
		HeartbeatAccumulator = 0.0f;
	}

	// Send position setpoints for offboard mode (required to maintain offboard mode)
	static float SetpointAccumulator = 0.0f;
	SetpointAccumulator += FixedDeltaTime;
	if (SetpointAccumulator >= 0.1f) // 10Hz position setpoints
	{
		SendPositionSetpoint();
		SetpointAccumulator = 0.0f;
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
	}
    if (!CommunicationThread)
    {
        CommunicationThread = new FPX4CommunicationThread(this);
        CommunicationThread->StartThread();
        UE_LOG(LogPX4, Warning, TEXT("Started PX4 communication thread:"));
        UE_LOG(LogPX4, Warning, TEXT("  - Mode: %s"), bUseLockstep ? TEXT("LOCKSTEP") : TEXT("REALTIME"));
        UE_LOG(LogPX4, Warning, TEXT("  - Frequency: 250Hz (4ms interval)"));
        UE_LOG(LogPX4, Warning, TEXT("  - Priority: TimeCritical"));
        UE_LOG(LogPX4, Warning, TEXT("  - Frame-independent: YES"));
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
	UpdateCurrentState();
}

void UPX4Component::SendHILDataFromThread()
{
	// In lockstep mode, this is handled by ThreadSimulationStep
	if (bUseLockstep)
	{
		return;
	}
	
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
		CurrentPosition		   = ThreadSafePosition;
		CurrentVelocity		   = ThreadSafeVelocity;
		CurrentRotation		   = ThreadSafeRotation;
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
    
	int32 TotalBytesSent = 0;
	const uint8* DataToSend = MessageBuffer;
	int32 BytesRemaining = MessageLength;
    
	// Use non-blocking send
	TCPClientSocket->SetNonBlocking(true);
    
	while (BytesRemaining > 0)
	{
		int32 BytesSent = 0;
		bool bSent = TCPClientSocket->Send(DataToSend + TotalBytesSent, BytesRemaining, BytesSent);
        
		if (!bSent || BytesSent <= 0)
		{
			// Check if it's just EWOULDBLOCK (temporary)
			ESocketErrors LastError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
			if (LastError == SE_EWOULDBLOCK)
			{
				// Socket buffer full, wait a bit
				FPlatformProcess::Sleep(0.0001f);
				continue;
			}
            
			ConsecutiveSendFailures++;
			UE_LOG(LogPX4, Error, TEXT("Failed to send MAVLink message ID=%d, error=%s, failures=%d"), 
				   msgid, ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(LastError), 
				   ConsecutiveSendFailures);
			break;
		}
        
		TotalBytesSent += BytesSent;
		BytesRemaining -= BytesSent;
		ConsecutiveSendFailures = 0;
	}
    
	// Log successful sends for debugging
	if (BytesRemaining == 0 && (MessageCounts[msgid] <= 3))
	{
		UE_LOG(LogPX4, Warning, TEXT("Successfully sent %d bytes for msg ID=%d"), 
			   TotalBytesSent, msgid);
	}
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
                        
                        // Send ACK with proper target information
                        mavlink_message_t ack_msg;
                        mavlink_command_ack_t ack;
                        ack.command = cmd.command;
                        ack.result = MAV_RESULT_ACCEPTED;
                        ack.target_system = msg.sysid; // ACK back to sender
                        ack.target_component = msg.compid; // ACK back to sender component

                        mavlink_msg_command_ack_encode(SystemID, ComponentID, &ack_msg, &ack);

                        uint8 buffer[MAVLINK_MAX_PACKET_LEN];
                        uint16 len = mavlink_msg_to_send_buffer(buffer, &ack_msg);

                        UE_LOG(LogPX4, Warning, TEXT("Sending COMMAND_ACK for cmd=%d to sys=%d comp=%d"),
                               ack.command, ack.target_system, ack.target_component);

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

	heartbeat.type = MAV_TYPE_GCS; // Ground Control Station
	heartbeat.autopilot = MAV_AUTOPILOT_INVALID;
	heartbeat.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED |
						 MAV_MODE_FLAG_HIL_ENABLED |
						 MAV_MODE_FLAG_SAFETY_ARMED |
						 MAV_MODE_FLAG_TEST_ENABLED;
	heartbeat.custom_mode = 0;
	heartbeat.system_status = MAV_STATE_ACTIVE;
	heartbeat.mavlink_version = 3; // MAVLink 2.0

	mavlink_msg_heartbeat_encode(SystemID, ComponentID, &msg, &heartbeat);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	static int32 HeartbeatCount = 0;
	if (++HeartbeatCount % 10 == 1) // Log every 10th heartbeat (every 5 seconds)
	{
		UE_LOG(LogPX4, Warning, TEXT("Sending heartbeat #%d: type=%d, base_mode=0x%X"),
			   HeartbeatCount, heartbeat.type, heartbeat.base_mode);
	}

	SendMAVLinkMessage(buffer, len);

	// Staged initialization sequence for PX4
	if (HeartbeatCount == 5) // After ~2.5 seconds
	{
		SendGPSOriginCommand();
	}
	else if (HeartbeatCount == 10) // After ~5 seconds
	{
		SendAttitudeResetCommand();
	}
	else if (HeartbeatCount == 15) // After ~7.5 seconds
	{
		SendEKF2ResetCommand();
	}
	else if (HeartbeatCount == 25) // After ~12.5 seconds
	{
		SendOffboardModeCommand();
	}
}

void UPX4Component::SendOffboardModeCommand()
{
	UE_LOG(LogPX4, Warning, TEXT("Attempting to enable OFFBOARD mode for autonomous flight"));

	// Send command to set mode to OFFBOARD
	mavlink_message_t msg;
	mavlink_command_long_t cmd;

	cmd.target_system = TargetSystem;
	cmd.target_component = TargetComponent;
	cmd.command = MAV_CMD_DO_SET_MODE;
	cmd.confirmation = 0;
	cmd.param1 = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_SAFETY_ARMED; // base mode
	cmd.param2 = 6; // PX4_CUSTOM_MAIN_MODE_OFFBOARD
	cmd.param3 = 0; // sub mode
	cmd.param4 = 0;
	cmd.param5 = 0;
	cmd.param6 = 0;
	cmd.param7 = 0;

	mavlink_msg_command_long_encode(SystemID, ComponentID, &msg, &cmd);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	SendMAVLinkMessage(buffer, len);

	UE_LOG(LogPX4, Warning, TEXT("Sent OFFBOARD mode command to PX4"));
}

void UPX4Component::SendGPSOriginCommand()
{
	UE_LOG(LogPX4, Warning, TEXT("Setting GPS origin/home position for PX4"));

	// Send GPS global origin message
	mavlink_message_t msg;
	mavlink_set_gps_global_origin_t origin;

	origin.target_system = TargetSystem;
	origin.latitude = (int32_t)(47.6174755 * 1e7); // Seattle coordinates
	origin.longitude = (int32_t)(-122.3137982 * 1e7);
	origin.altitude = 100000; // 100m in mm
	origin.time_usec = LockstepCounter * 4000;

	mavlink_msg_set_gps_global_origin_encode(SystemID, ComponentID, &msg, &origin);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	SendMAVLinkMessage(buffer, len);
	UE_LOG(LogPX4, Warning, TEXT("Sent GPS origin: Lat=%.6f, Lon=%.6f, Alt=100m"),
		   47.6174755, -122.3137982);
}

void UPX4Component::SendAttitudeResetCommand()
{
	UE_LOG(LogPX4, Warning, TEXT("Sending attitude estimator reset command"));

	mavlink_message_t msg;
	mavlink_command_long_t cmd;

	cmd.target_system = TargetSystem;
	cmd.target_component = TargetComponent;
	cmd.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
	cmd.confirmation = 0;
	cmd.param1 = 0; // No autopilot reboot
	cmd.param2 = 0; // No companion computer reboot
	cmd.param3 = 0; // No camera reboot
	cmd.param4 = 2; // Reset attitude estimation only
	cmd.param5 = 0;
	cmd.param6 = 0;
	cmd.param7 = 0;

	mavlink_msg_command_long_encode(SystemID, ComponentID, &msg, &cmd);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	SendMAVLinkMessage(buffer, len);
}

void UPX4Component::SendEKF2ResetCommand()
{
	UE_LOG(LogPX4, Warning, TEXT("Sending EKF2 reset command"));

	mavlink_message_t msg;
	mavlink_command_long_t cmd;

	cmd.target_system = TargetSystem;
	cmd.target_component = TargetComponent;
	cmd.command = MAV_CMD_PREFLIGHT_CALIBRATION;
	cmd.confirmation = 0;
	cmd.param1 = 0; // No gyro calibration
	cmd.param2 = 0; // No mag calibration
	cmd.param3 = 0; // No ground pressure calibration
	cmd.param4 = 0; // No radio calibration
	cmd.param5 = 2; // Reset attitude estimation
	cmd.param6 = 0; // No acceleration calibration
	cmd.param7 = 0; // No airspeed calibration

	mavlink_msg_command_long_encode(SystemID, ComponentID, &msg, &cmd);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	SendMAVLinkMessage(buffer, len);
}

void UPX4Component::SendPositionSetpoint()
{
	// Send position setpoint to maintain offboard mode
	// This tells PX4 to hold position at current location
	mavlink_message_t msg;
	mavlink_set_position_target_local_ned_t setpoint;

	setpoint.time_boot_ms = LockstepCounter * 4; // Convert to milliseconds
	setpoint.target_system = TargetSystem;
	setpoint.target_component = TargetComponent;
	setpoint.coordinate_frame = MAV_FRAME_LOCAL_NED;

	// Position hold at current position (in NED frame)
	setpoint.x = CurrentPosition.X;
	setpoint.y = CurrentPosition.Y;
	setpoint.z = CurrentPosition.Z - 1.0f; // Hover 1 meter above ground

	// Zero velocity (position hold)
	setpoint.vx = 0.0f;
	setpoint.vy = 0.0f;
	setpoint.vz = 0.0f;

	// Zero acceleration
	setpoint.afx = 0.0f;
	setpoint.afy = 0.0f;
	setpoint.afz = 0.0f;

	// Zero yaw rate, maintain current yaw
	setpoint.yaw = FMath::DegreesToRadians(CurrentRotation.Yaw);
	setpoint.yaw_rate = 0.0f;

	// Use position and yaw control
	setpoint.type_mask =
		POSITION_TARGET_TYPEMASK_VX_IGNORE |
		POSITION_TARGET_TYPEMASK_VY_IGNORE |
		POSITION_TARGET_TYPEMASK_VZ_IGNORE |
		POSITION_TARGET_TYPEMASK_AX_IGNORE |
		POSITION_TARGET_TYPEMASK_AY_IGNORE |
		POSITION_TARGET_TYPEMASK_AZ_IGNORE |
		POSITION_TARGET_TYPEMASK_YAW_RATE_IGNORE;

	mavlink_msg_set_position_target_local_ned_encode(SystemID, ComponentID, &msg, &setpoint);

	uint8 buffer[MAVLINK_MAX_PACKET_LEN];
	uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);

	SendMAVLinkMessage(buffer, len);

	static int32 SetpointCount = 0;
	if (++SetpointCount % 100 == 1) // Log every 100th setpoint (every 10 seconds)
	{
		UE_LOG(LogPX4, Log, TEXT("Sent position setpoint #%d: [%.2f, %.2f, %.2f] yaw=%.1f°"),
			   SetpointCount, setpoint.x, setpoint.y, setpoint.z, FMath::RadiansToDegrees(setpoint.yaw));
	}
}


void UPX4Component::SendHILStateQuaternion()
{
    mavlink_message_t msg;
    mavlink_hil_state_quaternion_t hil_state;
    
    memset(&hil_state, 0, sizeof(hil_state));
    
    // Use same timestamp as HIL_SENSOR for consistency
    uint64_t timestamp_us = LockstepCounter * 4000;
    hil_state.time_usec = timestamp_us;
    
    // Convert quaternion (CurrentRotation is already in NED from UpdateCurrentState)
	FQuat NEDQuat = FQuat(CurrentRotation);

    // CRITICAL DEBUG: Disable forced identity quaternion now that we have proper init
    static bool bForceIdentityQuat = false;
    if (bForceIdentityQuat)
    {
        UE_LOG(LogPX4, Error, TEXT("ORIGINAL quaternion: W=%.3f, X=%.3f, Y=%.3f, Z=%.3f"),
               NEDQuat.W, NEDQuat.X, NEDQuat.Y, NEDQuat.Z);
        NEDQuat = FQuat::Identity; // Force identity quaternion (level attitude)
        UE_LOG(LogPX4, Error, TEXT("FORCING IDENTITY quaternion: W=%.3f, X=%.3f, Y=%.3f, Z=%.3f"),
               NEDQuat.W, NEDQuat.X, NEDQuat.Y, NEDQuat.Z);
    }
    else
    {
        // Normal quaternion validation (disabled for now)
        if (FMath::IsNaN(NEDQuat.W) || FMath::IsNaN(NEDQuat.X) ||
            FMath::IsNaN(NEDQuat.Y) || FMath::IsNaN(NEDQuat.Z))
        {
            UE_LOG(LogPX4, Warning, TEXT("Invalid quaternion detected, using identity"));
            NEDQuat = FQuat::Identity; // Identity quaternion (no rotation)
        }
        else
        {
            // Ensure quaternion is normalized
            NEDQuat.Normalize();

            // Check if normalization was successful
            if (!NEDQuat.IsNormalized())
            {
                UE_LOG(LogPX4, Warning, TEXT("Failed to normalize quaternion, using identity"));
                NEDQuat = FQuat::Identity;
            }
        }
    }

	// Fill quaternion array
	hil_state.attitude_quaternion[0] = NEDQuat.W;  // w
	hil_state.attitude_quaternion[1] = NEDQuat.X;  // x
	hil_state.attitude_quaternion[2] = NEDQuat.Y;  // y
	hil_state.attitude_quaternion[3] = NEDQuat.Z;  // z
    
    // Angular velocities (rad/s)
	hil_state.rollspeed = CurrentAngularVelocity.X;
	hil_state.pitchspeed = CurrentAngularVelocity.Y;
	hil_state.yawspeed = CurrentAngularVelocity.Z;
    
    // GPS position (using fixed location for now)
	hil_state.lat = (int32_t)(CurrentGeoCoords.X * 1e7);
	hil_state.lon = (int32_t)(CurrentGeoCoords.Y * 1e7);
	hil_state.alt = (int32_t)(CurrentGeoCoords.Z * 1000); // mm
	
    // Velocities in m/s
	hil_state.vx = (int16_t)(CurrentVelocity.X);
	hil_state.vy = (int16_t)(CurrentVelocity.Y);
	hil_state.vz = (int16_t)(CurrentVelocity.Z);
    
    // Ground speed
	float ground_speed_ms = FMath::Sqrt(CurrentVelocity.X * CurrentVelocity.X + CurrentVelocity.Y * CurrentVelocity.Y);
	hil_state.ind_airspeed = (uint16_t)(ground_speed_ms); // m/s
	hil_state.true_airspeed = hil_state.ind_airspeed;
    
    // Accelerations (use gravity-compensated values)
	hil_state.xacc = (int16_t)(CurrentAccelData.X * 100); // Convert m/s^2 to cm/s^2
	hil_state.yacc = (int16_t)(CurrentAccelData.Y * 100);
	hil_state.zacc = (int16_t)(CurrentAccelData.Z * 100);
    
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
    
	AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner());
	if (!QuadPawn || !QuadPawn->SensorManager)
	{
		UE_LOG(LogPX4, Error, TEXT("No QuadPawn or SensorManager found, cannot send HIL sensor data"));
		return; // Don't send invalid messages
	}

	// Validate that sensors are actually providing data
	if (!QuadPawn->SensorManager->GPS || !QuadPawn->SensorManager->IMU ||
		!QuadPawn->SensorManager->Magnetometer || !QuadPawn->SensorManager->Barometer)
	{
		UE_LOG(LogPX4, Error, TEXT("One or more sensors are missing in SensorManager"));
		return;
	}


	// For PX4, we need acceleration INCLUDING gravity (specific force)
	// CurrentAccelData is already in NED frame and in m/s^2 from coordinate transform
	// The IMU should include gravity in the Z-axis when the drone is level
	FQuat DroneQuat = FQuat(CurrentRotation); // Already in NED from coordinate transform
	FVector GravityNED(0, 0, 9.81f); // Down is positive in NED
	FVector GravityBody = DroneQuat.UnrotateVector(GravityNED);

    // Accelerometer (m/s^2) - includes gravity (specific force measurement)
    // Validate acceleration data
    FVector TotalAccel = CurrentAccelData + GravityBody;

    // Check for NaN or unreasonable acceleration values
    if (FMath::IsNaN(TotalAccel.X) || FMath::Abs(TotalAccel.X) > 50.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid X acceleration: %.2f, using 0"), TotalAccel.X);
        TotalAccel.X = 0.0f;
    }
    if (FMath::IsNaN(TotalAccel.Y) || FMath::Abs(TotalAccel.Y) > 50.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid Y acceleration: %.2f, using 0"), TotalAccel.Y);
        TotalAccel.Y = 0.0f;
    }
    if (FMath::IsNaN(TotalAccel.Z) || FMath::Abs(TotalAccel.Z) > 50.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid Z acceleration: %.2f, using 9.81"), TotalAccel.Z);
        TotalAccel.Z = 9.81f; // Default gravity when stationary
    }

	hil_sensor.xacc = TotalAccel.X;
	hil_sensor.yacc = TotalAccel.Y;
	hil_sensor.zacc = TotalAccel.Z;
    
    // Gyroscope (rad/s) - validate angular velocity data
    FVector ValidatedAngVel = CurrentAngularVelocity;

    if (FMath::IsNaN(ValidatedAngVel.X) || FMath::Abs(ValidatedAngVel.X) > 100.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid X angular velocity: %.2f, using 0"), ValidatedAngVel.X);
        ValidatedAngVel.X = 0.0f;
    }
    if (FMath::IsNaN(ValidatedAngVel.Y) || FMath::Abs(ValidatedAngVel.Y) > 100.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid Y angular velocity: %.2f, using 0"), ValidatedAngVel.Y);
        ValidatedAngVel.Y = 0.0f;
    }
    if (FMath::IsNaN(ValidatedAngVel.Z) || FMath::Abs(ValidatedAngVel.Z) > 100.0f)
    {
        UE_LOG(LogPX4, Warning, TEXT("Invalid Z angular velocity: %.2f, using 0"), ValidatedAngVel.Z);
        ValidatedAngVel.Z = 0.0f;
    }

	hil_sensor.xgyro = ValidatedAngVel.X;
	hil_sensor.ygyro = ValidatedAngVel.Y;
	hil_sensor.zgyro = ValidatedAngVel.Z;
    
    // Magnetometer (Gauss) - normalized earth field
	hil_sensor.xmag = CurrentMagData.X;
	hil_sensor.ymag = CurrentMagData.Y;
	hil_sensor.zmag = CurrentMagData.Z;

	// Validate pressure data (must be reasonable atmospheric pressure)
	float PressureHPa = CurrentPressure / 100.0f;
	if (PressureHPa < 300.0f || PressureHPa > 1200.0f || FMath::IsNaN(PressureHPa))
	{
		UE_LOG(LogPX4, Warning, TEXT("Invalid pressure data: %.2f hPa, using default 1013.25"), PressureHPa);
		PressureHPa = 1013.25f; // Sea level standard
	}
	hil_sensor.abs_pressure = PressureHPa;
	hil_sensor.diff_pressure = 0.0f; // No airspeed sensor

	// Validate altitude
	float ValidatedAltitude = CurrentAltitude;
	if (FMath::IsNaN(ValidatedAltitude) || FMath::Abs(ValidatedAltitude) > 10000.0f)
	{
		UE_LOG(LogPX4, Warning, TEXT("Invalid altitude: %.2f, using 0"), ValidatedAltitude);
		ValidatedAltitude = 0.0f;
	}
	hil_sensor.pressure_alt = ValidatedAltitude;

	// Validate temperature
	float ValidatedTemp = CurrentTemperature;
	if (FMath::IsNaN(ValidatedTemp) || ValidatedTemp < -100.0f || ValidatedTemp > 100.0f)
	{
		UE_LOG(LogPX4, Warning, TEXT("Invalid temperature: %.2f, using 20°C"), ValidatedTemp);
		ValidatedTemp = 20.0f;
	}
	hil_sensor.temperature = ValidatedTemp;
    
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
    
    // Set sensor instance IDs for different sensor types
    // PX4 expects: 0 = main IMU/accel/gyro, 1 = baro, 2 = mag, etc.
    hil_sensor.id = 0; // Use 0 for main sensor suite (IMU/accel/gyro/mag/baro combined)
    
    // Encode the message
	uint16 msg_len = mavlink_msg_hil_sensor_encode(SystemID, ComponentID, &msg, &hil_sensor);
    
    // Create buffer and serialize
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    // CRITICAL DEBUG: Check for sensor data staleness
	static int32 DbgCounter = 0;
	static uint64_t LastTimestamp = 0;
	static int32 StaleDataCounter = 0;

	uint64_t TimeDelta = hil_sensor.time_usec - LastTimestamp;
	if (LastTimestamp > 0 && TimeDelta != 4000) // Should be exactly 4ms in lockstep
	{
		StaleDataCounter++;
		UE_LOG(LogPX4, Error, TEXT("SENSOR TIMING ERROR: Expected 4000us, got %llu us (gap=%llu)"),
			   TimeDelta, hil_sensor.time_usec - LastTimestamp);
	}
	LastTimestamp = hil_sensor.time_usec;

	if (++DbgCounter % 50 == 0)
	{
		UE_LOG(LogPX4, Warning, TEXT("=== HIL_SENSOR[%d] TIMING CHECK ==="), DbgCounter);
		UE_LOG(LogPX4, Warning, TEXT("Time: %llu us, Delta: %llu us, Stale errors: %d"),
			   hil_sensor.time_usec, TimeDelta, StaleDataCounter);
		UE_LOG(LogPX4, Warning, TEXT("Fields: 0x%X, Lockstep: %s"),
			   hil_sensor.fields_updated, bUseLockstep ? TEXT("ON") : TEXT("OFF"));
		UE_LOG(LogPX4, Warning, TEXT("Accel: %.3f,%.3f,%.3f | Gyro: %.3f,%.3f,%.3f"),
			   hil_sensor.xacc, hil_sensor.yacc, hil_sensor.zacc,
			   hil_sensor.xgyro, hil_sensor.ygyro, hil_sensor.zgyro);
		UE_LOG(LogPX4, Warning, TEXT("Mag: %.3f,%.3f,%.3f | Press: %.1fmbar"),
			   hil_sensor.xmag, hil_sensor.ymag, hil_sensor.zmag, hil_sensor.abs_pressure);
		UE_LOG(LogPX4, Warning, TEXT("==========================="));
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
	hil_gps.lat = (int32_t)(CurrentGeoCoords.X * 1e7); // Convert to int32 * 1e7
	hil_gps.lon = (int32_t)(CurrentGeoCoords.Y * 1e7); // Convert to int32 * 1e7
	hil_gps.alt = (int32_t)(CurrentGeoCoords.Z * 1000); // Altitude in mm
	
	// GPS accuracy
	hil_gps.eph = 100; // HDOP * 100
	hil_gps.epv = 100; // VDOP * 100
    
	// Velocities in m/s
	hil_gps.vn = (int16_t)(CurrentVelocity.X); // North velocity m/s
	hil_gps.ve = (int16_t)(CurrentVelocity.Y); // East velocity m/s
	hil_gps.vd = (int16_t)(CurrentVelocity.Z); // Down velocity m/s
    
	// Ground speed and course
	float ground_speed_ms = FMath::Sqrt(CurrentVelocity.X * CurrentVelocity.X + CurrentVelocity.Y * CurrentVelocity.Y);
	hil_gps.vel = (uint16_t)(ground_speed_ms * 100.0f); // m/s
	
	// Course over ground in centidegrees (0-35999)
	float cog_rad = FMath::Atan2(CurrentVelocity.Y, CurrentVelocity.X); // East, North for NED
	float cog_deg = FMath::RadiansToDegrees(cog_rad);
	if (cog_deg < 0) cog_deg += 360.0f; // Normalize to 0-360
	hil_gps.cog = (uint16_t)(cog_deg * 100.0f); // centidegrees    
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
	// Set up for autonomous mode operation
	hil_rc.chan1_raw = 1500; // Roll (center)
	hil_rc.chan2_raw = 1500; // Pitch (center)
	hil_rc.chan3_raw = 1500; // Throttle (center for autonomous mode)
	hil_rc.chan4_raw = 1500; // Yaw (center)
	hil_rc.chan5_raw = 2000; // Mode switch (high = autonomous/offboard mode)
	hil_rc.chan6_raw = 1500; // Aux
	hil_rc.chan7_raw = 1500; // Arm switch (center/high = armed when in autonomous)
	hil_rc.chan8_raw = 2000; // Kill switch (high = normal operation)
	hil_rc.chan9_raw = 1500;
	hil_rc.chan10_raw = 1500;
	hil_rc.chan11_raw = 1500;
	hil_rc.chan12_raw = 1500;
    
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
	FMotorCommand NewCommand;
	NewCommand.Commands.SetNum(4);
	NewCommand.Timestamp = FPlatformTime::Seconds();
    
	// Log raw actuator controls from PX4
	UE_LOG(LogPX4, Warning, TEXT("=== PX4 Actuator Controls Received ==="));
	UE_LOG(LogPX4, Warning, TEXT("Raw actuator_controls from PX4: [%.4f, %.4f, %.4f, %.4f]"), 
		   actuator_controls.controls[0], actuator_controls.controls[1], 
		   actuator_controls.controls[2], actuator_controls.controls[3]);
    
	// PX4 sends normalized values (-1 to 1), convert to (0 to 1) for thrust
	for (int32 i = 0; i < 4 && i < 16; i++)
	{
		// Clamp and normalize from [-1,1] to [0,1]
		float NormalizedValue = FMath::Clamp((actuator_controls.controls[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
		NewCommand.Commands[i] = NormalizedValue;
		
		UE_LOG(LogPX4, Warning, TEXT("Motor %d: Raw=%.4f, Normalized=%.4f"), 
			   i, actuator_controls.controls[i], NormalizedValue);
	}
	
	UE_LOG(LogPX4, Warning, TEXT("======================================"));
    
	// Queue the command for game thread processing
	PendingMotorCommands.Enqueue(NewCommand);
    
	static int32 ActuatorCount = 0;
	if (++ActuatorCount % 100 == 0) // Log every 100th message
	{
		UE_LOG(LogPX4, Log, TEXT("Queued motor commands #%d: M1=%.3f, M2=%.3f, M3=%.3f, M4=%.3f"), 
			   ActuatorCount, NewCommand.Commands[0], NewCommand.Commands[1], 
			   NewCommand.Commands[2], NewCommand.Commands[3]);
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
		// CRITICAL: Always log sensor status to debug initialization issues
		static int32 SensorStatusCounter = 0;
		if (++SensorStatusCounter % 50 == 1)
		{
			UE_LOG(LogPX4, Error, TEXT("=== SENSOR STATUS CHECK ==="));
			UE_LOG(LogPX4, Error, TEXT("SensorManager: %s"), QuadPawn->SensorManager ? TEXT("VALID") : TEXT("NULL"));
			if (QuadPawn->SensorManager)
			{
				UE_LOG(LogPX4, Error, TEXT("GPS: %s"), QuadPawn->SensorManager->GPS ? TEXT("VALID") : TEXT("NULL"));
				UE_LOG(LogPX4, Error, TEXT("IMU: %s"), QuadPawn->SensorManager->IMU ? TEXT("VALID") : TEXT("NULL"));
				UE_LOG(LogPX4, Error, TEXT("Barometer: %s"), QuadPawn->SensorManager->Barometer ? TEXT("VALID") : TEXT("NULL"));
				UE_LOG(LogPX4, Error, TEXT("Magnetometer: %s"), QuadPawn->SensorManager->Magnetometer ? TEXT("VALID") : TEXT("NULL"));
			}
			UE_LOG(LogPX4, Error, TEXT("==============================="));
		}

		// Get sensor data with fallback values
		FVector GPSPositionMeters = QuadPawn->SensorManager->GPS ? QuadPawn->SensorManager->GPS->GetLastGPS() : FVector::ZeroVector;
		FVector GeographicCoords = QuadPawn->SensorManager->GPS ? QuadPawn->SensorManager->GPS->GetGeographicCoordinates() : FVector(47.6174755, -122.3137982, 100.0f); // Default Seattle coords with altitude

		// FORCE realistic GPS coordinates for initial testing
		if (GeographicCoords.X == 0.0f && GeographicCoords.Y == 0.0f)
		{
			GeographicCoords = FVector(47.6174755, -122.3137982, 100.0f); // Force Seattle coordinates
			UE_LOG(LogPX4, Warning, TEXT("FORCING GPS coordinates to Seattle: %.6f, %.6f, %.1f"),
				   GeographicCoords.X, GeographicCoords.Y, GeographicCoords.Z);
		}
		FVector IMUVelocity = QuadPawn->SensorManager->IMU ? QuadPawn->SensorManager->IMU->GetLastVelocity() : FVector::ZeroVector;
		FVector AccelData = QuadPawn->SensorManager->IMU ? QuadPawn->SensorManager->IMU->GetLastAccelerometer() : FVector(0, 0, 9.81f);
		FRotator IMUAttitude = QuadPawn->SensorManager->IMU ? QuadPawn->SensorManager->IMU->GetLastAttitude() : FRotator::ZeroRotator;
		FVector IMUAngularVelDeg = QuadPawn->SensorManager->IMU ? QuadPawn->SensorManager->IMU->GetLastGyroscopeDegrees() : FVector::ZeroVector;

		float Pressure = QuadPawn->SensorManager->Barometer ? QuadPawn->SensorManager->Barometer->GetLastPressure() : 101325.0f;
		float Temperature = QuadPawn->SensorManager->Barometer ? QuadPawn->SensorManager->Barometer->GetLastTemperature() : 20.0f;
		float BaroAltitude = QuadPawn->SensorManager->Barometer ? QuadPawn->SensorManager->Barometer->GetEstimatedAltitude() : 0.0f;

		FVector MagData = QuadPawn->SensorManager->Magnetometer ? QuadPawn->SensorManager->Magnetometer->GetLastMagField() : FVector(0.3f, 0.0f, 0.5f);

		// Check for sensor failures and provide fallback values
		bool bSensorFailure = false;

		// Check GPS data
		if (FMath::IsNaN(GPSPositionMeters.X) || FMath::IsNaN(GPSPositionMeters.Y) || FMath::IsNaN(GPSPositionMeters.Z))
		{
			UE_LOG(LogPX4, Error, TEXT("GPS sensor failure - NaN values detected"));
			GPSPositionMeters = FVector::ZeroVector;
			bSensorFailure = true;
		}

		// Check IMU acceleration - critical for attitude estimation
		if (FMath::IsNaN(AccelData.X) || FMath::IsNaN(AccelData.Y) || FMath::IsNaN(AccelData.Z))
		{
			UE_LOG(LogPX4, Error, TEXT("IMU accelerometer failure - NaN values detected, using gravity fallback"));
			AccelData = FVector(0, 0, 9.81f); // Stationary, level attitude
			bSensorFailure = true;
		}

		// CRITICAL DEBUG: Disable forced level attitude now that we have proper init sequence
		static bool bForceLevel = false;
		if (bForceLevel)
		{
			UE_LOG(LogPX4, Error, TEXT("ORIGINAL IMU attitude: R=%.2f, P=%.2f, Y=%.2f"),
				   IMUAttitude.Roll, IMUAttitude.Pitch, IMUAttitude.Yaw);
			IMUAttitude = FRotator(0.0f, 0.0f, 0.0f); // Completely level
			UE_LOG(LogPX4, Error, TEXT("FORCING LEVEL attitude: R=%.2f, P=%.2f, Y=%.2f"),
				   IMUAttitude.Roll, IMUAttitude.Pitch, IMUAttitude.Yaw);
			bSensorFailure = true;
		}
		else
		{
			// Normal attitude validation (disabled for now)
			bool bAttitudeInvalid = false;
			if (FMath::IsNaN(IMUAttitude.Roll) || FMath::IsNaN(IMUAttitude.Pitch) || FMath::IsNaN(IMUAttitude.Yaw))
			{
				UE_LOG(LogPX4, Error, TEXT("IMU attitude NaN detected: R=%.1f, P=%.1f, Y=%.1f"),
					   IMUAttitude.Roll, IMUAttitude.Pitch, IMUAttitude.Yaw);
				bAttitudeInvalid = true;
			}

			// Check if roll/pitch exceed PX4 safe limits (typically ±60 degrees)
			if (FMath::Abs(IMUAttitude.Roll) > 45.0f || FMath::Abs(IMUAttitude.Pitch) > 45.0f)
			{
				UE_LOG(LogPX4, Error, TEXT("IMU attitude exceeds safe limits: R=%.1f, P=%.1f, Y=%.1f"),
					   IMUAttitude.Roll, IMUAttitude.Pitch, IMUAttitude.Yaw);
				bAttitudeInvalid = true;
			}

			if (bAttitudeInvalid)
			{
				// Force level attitude for PX4 safety
				IMUAttitude = FRotator(0.0f, IMUAttitude.Yaw, 0.0f); // Keep yaw, zero roll/pitch
				UE_LOG(LogPX4, Warning, TEXT("Forcing level attitude for PX4 safety"));
				bSensorFailure = true;
			}
		}

		// Check angular velocity
		if (FMath::IsNaN(IMUAngularVelDeg.X) || FMath::IsNaN(IMUAngularVelDeg.Y) || FMath::IsNaN(IMUAngularVelDeg.Z))
		{
			UE_LOG(LogPX4, Error, TEXT("IMU gyroscope failure - NaN values detected"));
			IMUAngularVelDeg = FVector::ZeroVector;
			bSensorFailure = true;
		}

		// Check barometer
		if (FMath::IsNaN(Pressure) || Pressure < 50000.0f || Pressure > 120000.0f)
		{
			UE_LOG(LogPX4, Error, TEXT("Barometer failure - invalid pressure: %.1f Pa"), Pressure);
			Pressure = 101325.0f; // Sea level standard
			BaroAltitude = 0.0f;
			bSensorFailure = true;
		}

		// Debug sensor data (every 250 frames to reduce spam)
		static int32 SensorDbgCounter = 0;
		if (++SensorDbgCounter % 250 == 0 || bSensorFailure)
		{
			UE_LOG(LogPX4, Warning, TEXT("=== RAW SENSOR DATA (Frame %d) ==="), SensorDbgCounter);
			UE_LOG(LogPX4, Warning, TEXT("GPS Position (m): %.2f, %.2f, %.2f"), GPSPositionMeters.X, GPSPositionMeters.Y, GPSPositionMeters.Z);
			UE_LOG(LogPX4, Warning, TEXT("Geographic: Lat=%.6f, Lon=%.6f, Alt=%.2f"), GeographicCoords.X, GeographicCoords.Y, GeographicCoords.Z);
			UE_LOG(LogPX4, Warning, TEXT("IMU Velocity (m/s): %.2f, %.2f, %.2f"), IMUVelocity.X, IMUVelocity.Y, IMUVelocity.Z);
			UE_LOG(LogPX4, Warning, TEXT("IMU Accel (m/s²): %.2f, %.2f, %.2f"), AccelData.X, AccelData.Y, AccelData.Z);
			UE_LOG(LogPX4, Warning, TEXT("==> IMU Attitude RAW (deg): R=%.2f, P=%.2f, Y=%.2f"), IMUAttitude.Roll, IMUAttitude.Pitch, IMUAttitude.Yaw);

			// Show the transformed attitude that will be sent to PX4
			FRotator NEDAttitude = UCoordinateTransform::UnrealRotationToNED(IMUAttitude);
			UE_LOG(LogPX4, Warning, TEXT("==> NED Attitude TRANSFORMED (deg): R=%.2f, P=%.2f, Y=%.2f"), NEDAttitude.Roll, NEDAttitude.Pitch, NEDAttitude.Yaw);

			UE_LOG(LogPX4, Warning, TEXT("IMU AngVel (deg/s): %.2f, %.2f, %.2f"), IMUAngularVelDeg.X, IMUAngularVelDeg.Y, IMUAngularVelDeg.Z);
			UE_LOG(LogPX4, Warning, TEXT("Mag (Gauss): %.3f, %.3f, %.3f"), MagData.X, MagData.Y, MagData.Z);
			UE_LOG(LogPX4, Warning, TEXT("Baro: Press=%.1f Pa, Alt=%.2f m, Temp=%.1f C"), Pressure, BaroAltitude, Temperature);
			if (bSensorFailure)
			{
				UE_LOG(LogPX4, Error, TEXT("*** SENSOR FAILURE DETECTED - USING FALLBACK VALUES ***"));
			}
			UE_LOG(LogPX4, Warning, TEXT("======================="));
		}

		// Transform to NED coordinates
		CurrentPosition = UCoordinateTransform::UnrealToNED(GPSPositionMeters); // Now in NED meters
		CurrentVelocity = UCoordinateTransform::UnrealVelocityToNED(IMUVelocity); // Now in NED m/s
		// CRITICAL FIX: Transform accelerometer data from Unreal FLU body frame to PX4 FRD body frame
		CurrentAccelData = UCoordinateTransform::UnrealBodyAccelToFRD(AccelData); // Now in FRD m/s² body frame

		// DEBUG: Log coordinate frame transformation to verify gravity appears as negative Z
		static int32 DebugLogCounter = 0;
		if (DebugLogCounter % 250 == 0) // Log every second at 250Hz
		{
			UE_LOG(LogPX4, Warning, TEXT("COORD TRANSFORM DEBUG - FLU->FRD: AccelIn(%.3f,%.3f,%.3f) -> AccelOut(%.3f,%.3f,%.3f)"),
				AccelData.X, AccelData.Y, AccelData.Z,
				CurrentAccelData.X, CurrentAccelData.Y, CurrentAccelData.Z);
		}
		DebugLogCounter++;
		CurrentRotation = UCoordinateTransform::UnrealRotationToNED(IMUAttitude); // Now in NED frame
        
		// Angular velocity: IMU gives deg/s in FLU body frame, need rad/s in FRD body frame for PX4
		// Convert deg/s to rad/s and handle coordinate system conversion from FLU to FRD
		CurrentAngularVelocity = UCoordinateTransform::UnrealBodyAngVelToFRD(IMUAngularVelDeg);
        
		// Geographic coordinates stay the same (lat/lon/alt)
		CurrentGeoCoords = GeographicCoords;
		CurrentAltitude = BaroAltitude;

		CurrentMagData = MagData;

		CurrentPressure = Pressure;
		CurrentTemperature = Temperature;
		
		// Update thread-safe copies if needed
		FScopeLock Lock(&StateMutex);
		ThreadSafePosition = CurrentPosition;
		ThreadSafeVelocity = CurrentVelocity;
		ThreadSafeRotation = CurrentRotation;
		ThreadSafeAngularVelocity = CurrentAngularVelocity;
		ThreadSafeGeoCoords = CurrentGeoCoords;
		ThreadSafeMagData = CurrentMagData;
		ThreadSafeAccelData = CurrentAccelData;
		ThreadSafePressureData = CurrentPressure;
		ThreadSafeTemperatureData = CurrentTemperature;
		ThreadSafeAltitudeData = CurrentAltitude;
		
		bThreadSafeDataValid = true;
		
	}
}

void UPX4Component::SetLockstepMode(bool bEnabled)
{
	FScopeLock Lock(&LockstepMutex);
	
	if (bUseLockstep != bEnabled)
	{
		bUseLockstep = bEnabled;
		
		UE_LOG(LogPX4, Warning, TEXT("PX4 lockstep mode %s"), bEnabled ? TEXT("enabled") : TEXT("disabled"));
		
		// Reset timing when switching modes
		LastLockstepTime = 0.0;
		LockstepCounter = 1;
		SimulationStepCounter = 0;
		
		if (bEnabled)
		{
			UE_LOG(LogPX4, Warning, TEXT("Lockstep mode: PX4 will run at fixed 250Hz independent of frame rate"));
		}
		else
		{
			UE_LOG(LogPX4, Warning, TEXT("Realtime mode: PX4 will run at 250Hz with best-effort timing"));
		}
	}
}

void UPX4Component::ThreadSimulationStep()
{
	// This function is now called directly from the communications thread's 250Hz loop.
	// It is responsible for sending all periodic HIL data.

	FScopeLock Lock(&StateMutex);
	if (!bThreadSafeDataValid) return; // Don't send if we don't have fresh data

	// --- Update local state from the thread-safe copies ---
	CurrentPosition = ThreadSafePosition;
	CurrentVelocity = ThreadSafeVelocity;
	CurrentRotation = ThreadSafeRotation;
	CurrentAngularVelocity = ThreadSafeAngularVelocity;
    
	// --- Increment Counters ---
	// The timestamp is based on the lockstep counter, advancing by 4000us (4ms) each step.
	LockstepCounter++;
	SimulationStepCounter++;

	// Add timestamp debugging
	static uint64_t LastStepTimestamp = 0;
	uint64_t CurrentTimestamp = LockstepCounter * 4000;
	if (LastStepTimestamp > 0)
	{
		uint64_t TimeDelta = CurrentTimestamp - LastStepTimestamp;
		if (TimeDelta != 4000)
		{
			UE_LOG(LogPX4, Error, TEXT("LOCKSTEP TIMING ERROR: Expected 4000us, got %llu us"),
				   TimeDelta);
		}
	}
	LastStepTimestamp = CurrentTimestamp;

	// --- Send High-Frequency Data (250Hz) ---
	// These must be sent on every single step.
	SendHILSensor();
	SendHILStateQuaternion();

	// --- Send Lower-Frequency Data ---
	// Send GPS and RC inputs at 50Hz (every 5 steps).
	if (SimulationStepCounter % 5 == 0) 
	{
		SendHILGPS();
		SendHILRCInputs();
	}
    
	// --- Send Heartbeat (2Hz) ---
	// Send a heartbeat every 125 steps (250Hz / 2Hz = 125).
	if (SimulationStepCounter % 125 == 0)
	{
		SendHeartbeat();
	}
}