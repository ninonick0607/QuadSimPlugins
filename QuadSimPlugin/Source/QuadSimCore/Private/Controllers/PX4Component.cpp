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
}

void UPX4Component::BeginPlay()
{
    Super::BeginPlay();
    
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
    if (!TCPClientSocket) return;
    
    // Update timers
    HeartbeatTimer += DeltaTime;
    ConnectionTimeoutTimer += DeltaTime;
	if (bConnectedToPX4 && bUseLockstep)
	{
		// In lockstep mode, send one set of sensor data per frame
		UpdateCurrentState(); // Get latest state
		SendHILSensor();
		SendHILStateQuaternion();
        
		// Send GPS/RC less frequently
		static int32 SubFrameCounter = 0;
		if (++SubFrameCounter % 4 == 0) // Every 4th frame
		{
			SendHILGPS();
			SendHILRCInputs();
		}
	}
    
    // Process incoming MAVLink data
    ProcessIncomingMAVLinkData();
    
    // Send heartbeat (keep this in main thread)
    if (bTCPConnected && HeartbeatTimer >= (1.0f / HeartbeatRate))
    {
        SendHeartbeat();
        HeartbeatTimer = 0.0f;
    }
    
    // Update thread-safe state for the communication thread
    if (bConnectedToPX4)
    {
        UpdateThreadSafeState();
    }
    
    // Connection timeout handling
    if (bConnectedToPX4 && ConnectionTimeoutTimer > 10.0f) // Increased timeout to 10 seconds
    {
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection lost - no heartbeat received for %.1f seconds"), ConnectionTimeoutTimer);
        bConnectedToPX4 = false;
        
        // Stop communication thread
        if (CommunicationThread)
        {
            UE_LOG(LogPX4, Warning, TEXT("Stopping communication thread due to connection loss"));
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
    
    // Check if there's a pending connection
    bool bHasPendingConnection = false;
    if (TCPListenSocket->HasPendingConnection(bHasPendingConnection) && bHasPendingConnection)
    {
        // Accept the connection
        TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        TCPClientSocket = TCPListenSocket->Accept(*RemoteAddr, TEXT("PX4TCPClientSocket"));
        
        if (TCPClientSocket)
        {
            // Set socket options on the accepted connection
        	TCPClientSocket->SetNonBlocking(true);
        	TCPClientSocket->SetNoDelay(true); // Disable Nagle's algorithm
        
        	// Set send buffer size for lower latency
        	int32 SendBufferSize = 32768; // 32KB
        	TCPClientSocket->SetSendBufferSize(SendBufferSize, SendBufferSize);
        
        	// Set receive buffer size
        	int32 RecvBufferSize = 32768; // 32KB
        	TCPClientSocket->SetReceiveBufferSize(RecvBufferSize, RecvBufferSize);
        
        	bTCPConnected = true;
            ConnectionTimeoutTimer = 0.0f;
            
            // Store the PX4 address
            PX4TCPAddress = RemoteAddr;
            
            UE_LOG(LogPX4, Warning, TEXT("PX4 connected from %s"), *RemoteAddr->ToString(true));
            UE_LOG(LogPX4, Warning, TEXT("TCP connection established - continuing MAVLink over TCP"));
            
            // We can close the listen socket now that we have a connection
            if (TCPListenSocket)
            {
                TCPListenSocket->Close();
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(TCPListenSocket);
                TCPListenSocket = nullptr;
                bTCPListening = false;
            }
        }
        else
        {
            UE_LOG(LogPX4, Error, TEXT("Failed to accept TCP connection from PX4"));
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
    
	// Set socket to blocking mode temporarily for critical messages
	TCPClientSocket->SetNonBlocking(false);
    
	int32 TotalBytesSent = 0;
	const uint8* DataToSend = MessageBuffer;
	int32 BytesRemaining = MessageLength;
    
	// Ensure we send ALL bytes
	while (BytesRemaining > 0)
	{
		int32 BytesSent = 0;
		bool bSent = TCPClientSocket->Send(DataToSend + TotalBytesSent, BytesRemaining, BytesSent);
        
		if (!bSent || BytesSent <= 0)
		{
			ConsecutiveSendFailures++;
			if (ConsecutiveSendFailures > 10)
			{
				UE_LOG(LogPX4, Error, TEXT("Failed to send MAVLink message after %d failures - closing connection"), 
					   ConsecutiveSendFailures);
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
    
	// Set timeout for recv
	ESocketReceiveFlags::Type ReceiveFlags;
	uint8 TempBuffer[4096]; // Larger buffer
	int32 BytesRead = 0;
    
	// Non-blocking receive with peek first
	if (TCPClientSocket->Recv(TempBuffer, sizeof(TempBuffer), BytesRead, 
							  ESocketReceiveFlags::Peek))
	{
		if (BytesRead > 0)
		{
			// Actually read the data
			TCPClientSocket->Recv(TempBuffer, BytesRead, BytesRead);
			ParseMAVLinkData(TempBuffer, BytesRead);
		}
	}
    
	// Check socket health
	ESocketConnectionState State = TCPClientSocket->GetConnectionState();
	if (State != SCS_Connected)
	{
		UE_LOG(LogPX4, Error, TEXT("TCP socket in bad state: %d"), (int32)State);
		bTCPConnected = false;
	}
}

void UPX4Component::ParseMAVLinkData(const uint8* Data, int32 DataLength)
{
    // Parse MAVLink messages
    mavlink_message_t msg;
    mavlink_status_t status;
    
    for (int32 i = 0; i < DataLength; i++)
    {
        if (mavlink_parse_char(MAVLINK_COMM_0, Data[i], &msg, &status))
        {
            UE_LOG(LogPX4, VeryVerbose, TEXT("Received MAVLink msg ID: %d from sys:%d comp:%d"), msg.msgid, msg.sysid, msg.compid);
        	UE_LOG(LogPX4, VeryVerbose, TEXT("RX: msgid=%d, seq=%d, sysid=%d, compid=%d"), msg.msgid, msg.seq, msg.sysid, msg.compid);
            switch (msg.msgid)
            {
            case MAVLINK_MSG_ID_HEARTBEAT: // ID 0
                UE_LOG(LogPX4, Warning, TEXT("Got PX4 Heartbeat!"));

                HandleHeartbeat(reinterpret_cast<const uint8*>(&msg), sizeof(msg));
                break;
            case MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS: // ID 93
                HandleActuatorOutputs(reinterpret_cast<const uint8*>(&msg), sizeof(msg));
                break;
            case MAVLINK_MSG_ID_COMMAND_LONG: // ID 76
                {
                    mavlink_command_long_t cmd;
                    mavlink_msg_command_long_decode(&msg, &cmd);
                    if (cmd.command == MAV_CMD_SET_MESSAGE_INTERVAL)
                    {
                        int32 MessageID = static_cast<int32>(cmd.param1);
                        float IntervalUs = cmd.param2;
                        float Frequency = IntervalUs > 0 ? 1000000.0f / IntervalUs : 0.0f;
                        UE_LOG(LogPX4, Warning, TEXT("PX4 requested message interval for ID %d at %f Hz"), MessageID, Frequency);
                        
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
                // Silently ignore unknown messages to avoid spam
                break;
            }
        }
    }
}

void UPX4Component::SendHeartbeat()
{
    mavlink_message_t msg;
    mavlink_heartbeat_t heartbeat;
    
    heartbeat.type = MAV_TYPE_GCS; // Simulators identify as GCS
    heartbeat.autopilot = MAV_AUTOPILOT_INVALID; // No autopilot for simulator
    heartbeat.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED | MAV_MODE_FLAG_HIL_ENABLED;
    heartbeat.custom_mode = 0;
    heartbeat.system_status = MAV_STATE_ACTIVE;
    heartbeat.mavlink_version = MAVLINK_VERSION;
    
    mavlink_msg_heartbeat_encode(SystemID, ComponentID, &msg, &heartbeat);
    
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    SendMAVLinkMessage(buffer, len);
}


void UPX4Component::SendHILStateQuaternion()
{
	mavlink_message_t msg;
	mavlink_hil_state_quaternion_t hil_state;
    
	if (bUseLockstep)
	{
		// Use the SAME counter value as SendHILSensor
		hil_state.time_usec = LockstepCounter * 4000;
	}
	else
	{
		hil_state.time_usec = GetSynchronizedTimestamp();
	}
    
    // Convert coordinates from Unreal to PX4 (NED) coordinate system
    float x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate;
    ConvertUnrealToPX4Coordinates(CurrentPosition, CurrentVelocity, CurrentRotation, CurrentAngularVelocity,
                                 x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate);
    
    // Use the HIL timestamp for consistency
    
    // Attitude quaternion (w, x, y, z order in MAVLink)
    hil_state.attitude_quaternion[0] = q0; // w
    hil_state.attitude_quaternion[1] = q1; // x
    hil_state.attitude_quaternion[2] = q2; // y
    hil_state.attitude_quaternion[3] = q3; // z
    
    // Angular velocities (rad/s)
    hil_state.rollspeed = FMath::DegreesToRadians(rollRate);
    hil_state.pitchspeed = FMath::DegreesToRadians(pitchRate);
    hil_state.yawspeed = FMath::DegreesToRadians(-yawRate); // Flip for NED
    
    // GPS coordinates (use realistic values)
    hil_state.lat = 473566094; // Zurich latitude * 1E7
    hil_state.lon = 85190237;  // Zurich longitude * 1E7
    hil_state.alt = static_cast<int32>((-z + 50000) * 10); // Altitude in mm above sea level
    
    // Velocities (cm/s) - PX4 expects these in cm/s
    hil_state.vx = static_cast<int16>(vx * 100);
    hil_state.vy = static_cast<int16>(vy * 100);
    hil_state.vz = static_cast<int16>(vz * 100);
    
    // Airspeed (not used for multirotor)
    hil_state.ind_airspeed = 0;
    hil_state.true_airspeed = 0;
    
    // Accelerations (cm/s^2) - can be zero for basic simulation
    hil_state.xacc = 0;
    hil_state.yacc = 0;
    hil_state.zacc = -980; // Standard gravity in cm/s^2
    
    mavlink_msg_hil_state_quaternion_encode(SystemID, ComponentID, &msg, &hil_state);
    
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    SendMAVLinkMessage(buffer, len);
}


void UPX4Component::SendHILSensor()
{
	mavlink_message_t msg;
	mavlink_hil_sensor_t hil_sensor;
    
	if (bUseLockstep)
	{
		// In lockstep mode, use incremental timestamps
		LockstepCounter++;
		hil_sensor.time_usec = LockstepCounter * 4000; // 4ms per step = 250Hz
	}
	else
	{
		// Real-time mode
		hil_sensor.time_usec = GetSynchronizedTimestamp();
	}
    
    // Get drone state
    FQuat DroneQuat = FQuat(CurrentRotation);
    FVector GravityWorld(0, 0, -9.81f);
    FVector GravityBody = DroneQuat.UnrotateVector(GravityWorld);
    
    // Accelerometer (m/s^2) - includes gravity
    hil_sensor.xacc = GravityBody.X;
    hil_sensor.yacc = GravityBody.Y;
    hil_sensor.zacc = GravityBody.Z;
    
    // Gyroscope (rad/s) - use actual angular velocity
    hil_sensor.xgyro = FMath::DegreesToRadians(CurrentAngularVelocity.X);
    hil_sensor.ygyro = FMath::DegreesToRadians(CurrentAngularVelocity.Y);
    hil_sensor.zgyro = FMath::DegreesToRadians(-CurrentAngularVelocity.Z);
    
    // Magnetometer (Gauss) - simulate earth's magnetic field
    FVector MagWorld(0.2f, 0.0f, 0.4f); // Typical values
    FVector MagBody = DroneQuat.UnrotateVector(MagWorld);
    hil_sensor.xmag = MagBody.X;
    hil_sensor.ymag = MagBody.Y;
    hil_sensor.zmag = MagBody.Z;
    
    // Barometer - proper atmospheric model
    float AltitudeMeters = -CurrentPosition.Z / 100.0f; // Convert cm to m
    float Pressure = 101325.0f * FMath::Pow(1.0f - (0.0065f * AltitudeMeters) / 288.15f, 5.255f);
    hil_sensor.abs_pressure = Pressure / 100.0f; // Pa to mbar
    hil_sensor.diff_pressure = 0.0f; // No airspeed for multirotor
    hil_sensor.pressure_alt = AltitudeMeters;
    hil_sensor.temperature = 20.0f + (AltitudeMeters * -0.0065f); // Temperature lapse
    
    // Set sensor validity flags
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
    
    hil_sensor.id = 0;
	if (bUseLockstep)
	{
		hil_sensor.fields_updated |= (1 << 31); // Lockstep flag
	}
    mavlink_msg_hil_sensor_encode(SystemID, ComponentID, &msg, &hil_sensor);
    
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    SendMAVLinkMessage(buffer, len);
}


void UPX4Component::SendHILGPS()
{
    mavlink_message_t msg;
    mavlink_hil_gps_t hil_gps;
    
    // Convert coordinates  
    float x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate;
    ConvertUnrealToPX4Coordinates(CurrentPosition, CurrentVelocity, CurrentRotation, CurrentAngularVelocity,
                                 x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate);
    
    // Use HIL timestamp
    hil_gps.time_usec = FPlatformTime::Seconds() * 1e6;
    hil_gps.lat = 473566094; // Zurich coordinates (degrees * 1E7)
    hil_gps.lon = 85190237;
    hil_gps.alt = static_cast<int32>((-z + 50000) * 10); // mm above sea level
    hil_gps.eph = 100; // GPS HDOP horizontal dilution of position (cm)
    hil_gps.epv = 100; // GPS VDOP vertical dilution of position (cm) 
    hil_gps.vel = static_cast<uint16>(FVector(vx, vy, vz).Size() * 100); // GPS ground speed (cm/s)
    hil_gps.vn = static_cast<int16>(vx * 100); // GPS velocity north (cm/s)
    hil_gps.ve = static_cast<int16>(vy * 100); // GPS velocity east (cm/s)
    hil_gps.vd = static_cast<int16>(vz * 100); // GPS velocity down (cm/s)
    hil_gps.cog = 0; // Course over ground
    hil_gps.fix_type = 3; // 3D fix
    hil_gps.satellites_visible = 12; // Number of satellites visible
    
    mavlink_msg_hil_gps_encode(SystemID, ComponentID, &msg, &hil_gps);
    
    uint8 buffer[MAVLINK_MAX_PACKET_LEN];
    uint16 len = mavlink_msg_to_send_buffer(buffer, &msg);
    
    SendMAVLinkMessage(buffer, len);
}

void UPX4Component::SendHILRCInputs()
{
    mavlink_message_t msg;
    mavlink_hil_rc_inputs_raw_t hil_rc;
    
    // Use HIL timestamp
    hil_rc.time_usec = FPlatformTime::Seconds() * 1e6;
    hil_rc.chan1_raw = 1500; // Roll
    hil_rc.chan2_raw = 1500; // Pitch  
    hil_rc.chan3_raw = 1000; // Throttle (low for safety)
    hil_rc.chan4_raw = 1500; // Yaw
    hil_rc.chan5_raw = 1500; // Mode switch
    hil_rc.chan6_raw = 1500;
    hil_rc.chan7_raw = 1500;
    hil_rc.chan8_raw = 1500;
    hil_rc.chan9_raw = 1500;
    hil_rc.chan10_raw = 1500;
    hil_rc.chan11_raw = 1500;
    hil_rc.chan12_raw = 1500;
    hil_rc.rssi = 255; // Full signal strength
    
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
    // Reset connection timeout when we receive heartbeat from PX4
    ConnectionTimeoutTimer = 0.0f;
    LastHeartbeatTime = GetWorld()->GetTimeSeconds();
    
    mavlink_message_t* msg = (mavlink_message_t*)MessageBuffer;
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(msg, &heartbeat);
    
    // Mark as truly connected when we receive data from PX4
	if (!bConnectedToPX4)
	{
		bConnectedToPX4 = true;
        
		if (bUseLockstep)
		{
			UE_LOG(LogPX4, Warning, TEXT("PX4 connection established - using LOCKSTEP mode"));
			// DON'T start thread in lockstep mode
		}
		else
		{
			UE_LOG(LogPX4, Warning, TEXT("PX4 connection established - starting communication thread"));
			if (!CommunicationThread)
			{
				CommunicationThread = new FPX4CommunicationThread(this);
				CommunicationThread->StartThread();
			}
		}
        
		UpdateThreadSafeState();
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
