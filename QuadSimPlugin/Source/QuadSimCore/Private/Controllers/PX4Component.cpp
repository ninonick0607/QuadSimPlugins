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
    
    // High precision timing
    double LastTime = FPlatformTime::Seconds();
    double LastHeartbeatTime = FPlatformTime::Seconds();
    int32 MessageCount = 0;
    const double HeartbeatInterval = 0.5; // Send heartbeat every 0.5 seconds (2Hz)
    
    while (!bStopRequested)
    {
        double CurrentTime = FPlatformTime::Seconds();
        double DeltaTime = CurrentTime - LastTime;
        
        // Only send data if enough time has passed (precise 250Hz timing)
        if (DeltaTime >= TARGET_INTERVAL)
        {
            // Send HIL data from thread
            if (PX4Component && PX4Component->IsConnectedToPX4())
            {
                PX4Component->SendHILDataFromThread();
                MessageCount++;
                
                // Log every 1000 messages (4 seconds at 250Hz)
                if (MessageCount % 1000 == 0)
                {
                    UE_LOG(LogPX4, Log, TEXT("Communication thread: Sent %d HIL messages"), MessageCount);
                }
            }
            
            LastTime = CurrentTime;
        }
        
        // Small sleep to prevent excessive CPU usage while maintaining precision
        FPlatformProcess::Sleep(0.001f); // 1ms sleep
    }
    
    UE_LOG(LogPX4, Warning, TEXT("PX4 Communication thread stopping"));
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
        Thread = FRunnableThread::Create(this, TEXT("PX4CommunicationThread"), 0, TPri_AboveNormal);
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
    
    SendHILSensor();
    SendHILStateQuaternion();
    
    // Send GPS and RC at lower rate (10Hz)
    static int32 GPSCounter = 0;
    if (++GPSCounter % 10 == 0)
    {
        SendHILGPS();
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
            TCPClientSocket->SetNoDelay(true); // Disable Nagle's algorithm for low latency
            
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
        return; // Silently fail to avoid spam from thread
    }
    
    int32 BytesSent = 0;
    bool bSent = TCPClientSocket->Send(MessageBuffer, MessageLength, BytesSent);
    
    if (!bSent || BytesSent != MessageLength)
    {
        static int32 FailCount = 0;
        if (++FailCount % 100 == 0) // Log every 100th failure
        {
            UE_LOG(LogPX4, Warning, TEXT("Failed to send MAVLink message via TCP: %d bytes sent of %d (failure #%d)"), 
                   BytesSent, MessageLength, FailCount);
        }
    }
}

void UPX4Component::ProcessIncomingMAVLinkData()
{
    // Process TCP data if connected
    if (TCPClientSocket && bTCPConnected)
    {
        uint32 PendingDataSize = 0;
        if (TCPClientSocket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
        {
            TArray<uint8> ReceivedData;
            ReceivedData.SetNumUninitialized(FMath::Min(PendingDataSize, 65507u));
            int32 BytesRead = 0;
            
            bool bReceived = TCPClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);
            
            if (bReceived && BytesRead > 0)
            {
                ParseMAVLinkData(ReceivedData.GetData(), BytesRead);
            }
            else if (!bReceived)
            {
                // Check if connection was closed
                ESocketConnectionState State = TCPClientSocket->GetConnectionState();
                if (State != SCS_Connected)
                {
                    UE_LOG(LogPX4, Warning, TEXT("PX4 TCP connection lost"));
                    bTCPConnected = false;
                    bConnectedToPX4 = false;
                }
            }
        }
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
            UE_LOG(LogPX4, VeryVerbose, TEXT("Received MAVLink msg ID: %d from sys:%d comp:%d"), 
       msg.msgid, msg.sysid, msg.compid);
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
    
    // Convert coordinates from Unreal to PX4 (NED) coordinate system
    float x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate;
    ConvertUnrealToPX4Coordinates(CurrentPosition, CurrentVelocity, CurrentRotation, CurrentAngularVelocity,
                                 x, y, z, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate);
    
    // Use the HIL timestamp for consistency
    hil_state.time_usec = FPlatformTime::Seconds() * 1e6;
    
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
    
    // Use REAL TIME for non-lockstep
    hil_sensor.time_usec = FPlatformTime::Seconds() * 1e6;
    
    // Get drone state
    FQuat DroneQuat = FQuat(CurrentRotation);
    FVector GravityWorld(0, 0, -9.81f); // Gravity in NED frame
    FVector GravityBody = DroneQuat.UnrotateVector(GravityWorld);
    
    // Accelerometer (m/s^2)
    hil_sensor.xacc = GravityBody.X;    
    hil_sensor.yacc = GravityBody.Y;    
    hil_sensor.zacc = GravityBody.Z;   
    
    // Gyroscope (rad/s)
    hil_sensor.xgyro = FMath::DegreesToRadians(CurrentAngularVelocity.X);
    hil_sensor.ygyro = FMath::DegreesToRadians(CurrentAngularVelocity.Y);
    hil_sensor.zgyro = FMath::DegreesToRadians(-CurrentAngularVelocity.Z);
    
    // Magnetometer (Gauss)
    FVector MagWorld(0.2f, 0.0f, 0.4f);
    FVector MagBody = DroneQuat.UnrotateVector(MagWorld);
    hil_sensor.xmag = MagBody.X;   
    hil_sensor.ymag = MagBody.Y;   
    hil_sensor.zmag = MagBody.Z;   
    
    // Barometer
    float AltitudeMeters = -CurrentPosition.Z / 100.0f;
    float Pressure = 101325.0f * FMath::Pow(1.0f - (0.0065f * AltitudeMeters) / 288.15f, 5.255f);
    hil_sensor.abs_pressure = Pressure / 100.0f; // Convert to millibar
    hil_sensor.diff_pressure = 0.0f;
    hil_sensor.pressure_alt = AltitudeMeters;
    hil_sensor.temperature = 20.0f;
    
    hil_sensor.id = 0;
    hil_sensor.fields_updated = 0x1FFF; // All sensor fields
    
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
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection established - starting communication thread"));
        
        // Initialize HIL timestamp to 0 for lockstep
        HILTimestamp = 0;
        
        // Start the dedicated communication thread
        if (!CommunicationThread)
        {
            CommunicationThread = new FPX4CommunicationThread(this);
            CommunicationThread->StartThread();
        }
        
        // Send initial data burst to kickstart PX4
        UpdateThreadSafeState();
        // Don't send burst here - let the thread handle it
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