// QuadSimPlugin/Source/QuadSimCore/Private/Components/PX4Component.cpp
#include "Controllers/PX4Component.h"
#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "Engine/World.h"

// MavLinkCom includes
#include "MavLinkConnection.hpp"
#include "MavLinkMessages.hpp"
#include "MavLinkVehicle.hpp"
#include "common/mavlink.h"

// Include MAVLink protocol definitions
#include "mavlink/protocol.h"
#include "mavlink/common/common.h"

DEFINE_LOG_CATEGORY_STATIC(LogPX4, Log, All);


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
    
    // High precision timing for 100Hz HIL data
    const double TargetInterval = 1.0 / TARGET_FREQUENCY_HZ; // 10ms for 100Hz
    double LastTime = FPlatformTime::Seconds();
    
    while (!bStopRequested)
    {
        double CurrentTime = FPlatformTime::Seconds();
        double DeltaTime = CurrentTime - LastTime;
        
        // Send HIL data precisely at 100Hz
        if (DeltaTime >= TargetInterval)
        {
            // Send HIL data from thread
            if (PX4Component && PX4Component->IsConnectedToPX4())
            {
                PX4Component->SendHILDataFromThread();
            }
            
            LastTime = CurrentTime;
        }
        
        // Sleep for 1ms to maintain precision while being CPU-friendly
        FPlatformProcess::Sleep(0.001f);
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
    if (!Thread)
    {
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


UPX4Component::UPX4Component()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
    
    // CRITICAL: For lockstep mode, use exactly 100Hz for HIL data
    StateUpdateRate = 100.0f; // Must match PX4's expected rate
    HeartbeatRate = 1.0f;     // 1Hz heartbeat is sufficient
    bEnableLockstep = false;  // Disable lockstep initially to get basic connection working
    
    CommunicationThread = nullptr;
    bThreadSafeDataValid = false;
    
    // Initialize MavLinkCom objects
    MavLinkConn = nullptr;
    Vehicle = nullptr;
    bConnected = false;
    
    // Initialize lockstep state
    bLockstepActive = false;
    LockstepResetCount = 0;
    bReceivedActuatorControls = false;
    LastActuatorControlsTimestamp = 0;
    LastSentHILTimestamp = 0;
    LastLockstepUpdateTime = 0.0;
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
    
    // Update timers
    HeartbeatTimer += DeltaTime;
    ConnectionTimeoutTimer += DeltaTime;
    
    // Send heartbeat periodically (main thread)
    if (bConnected && HeartbeatTimer >= (1.0f / HeartbeatRate))
    {
        SendHeartbeat();
        HeartbeatTimer = 0.0f;
    }
    
    // Update thread-safe state for the communication thread
    if (bConnected)
    {
        UpdateThreadSafeState();
    }
    
    // Connection timeout handling
    if (bConnected && ConnectionTimeoutTimer > (ConnectionTimeout * 3))
    {
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection lost - no heartbeat received"));
        DisconnectFromPX4();
        
        // Stop communication thread
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
    if (bConnected) return;
    
    UE_LOG(LogPX4, Warning, TEXT("Connecting to PX4 via MavLinkCom on port %d"), PX4_Port);
    
    SetupMavLinkConnection();
}

void UPX4Component::DisconnectFromPX4()
{
    if (!bConnected) return;
    
    UE_LOG(LogPX4, Warning, TEXT("Disconnecting from PX4"));
    
    CleanupConnection();
    bConnected = false;
    bConnectedToPX4 = false;
}

void UPX4Component::SetupMavLinkConnection()
{
    UE_LOG(LogPX4, Warning, TEXT("Setting up MavLinkCom connection to %s:%d"), *PX4_IP, PX4_Port);
    
    try
    {
        // Create TCP connection to PX4 SITL
        std::string LocalAddr = "127.0.0.1";
        std::string RemoteAddr = TCHAR_TO_UTF8(*PX4_IP);
        
        MavLinkConn = mavlinkcom::MavLinkConnection::connectTcp("UE5QuadSim", LocalAddr, RemoteAddr, PX4_Port);
        
        if (MavLinkConn && MavLinkConn->isOpen())
        {
            UE_LOG(LogPX4, Warning, TEXT("MavLinkCom TCP connection established successfully"));
            
            // Subscribe to messages we're interested in
            MavLinkConn->subscribe([this](std::shared_ptr<mavlinkcom::MavLinkConnection> connection, const mavlinkcom::MavLinkMessage& msg) {
                OnMessageReceived(std::make_shared<mavlinkcom::MavLinkMessage>(msg));
            });
            
            bConnected = true;
            bConnectedToPX4 = false; // Will be set to true when we receive first heartbeat
            
            UE_LOG(LogPX4, Warning, TEXT("MavLinkCom connection ready, waiting for PX4 heartbeat..."));
        }
        else
        {
            UE_LOG(LogPX4, Error, TEXT("Failed to establish MavLinkCom TCP connection"));
            bConnected = false;
            bConnectedToPX4 = false;
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogPX4, Error, TEXT("Exception in SetupMavLinkConnection: %s"), UTF8_TO_TCHAR(e.what()));
        bConnected = false;
        bConnectedToPX4 = false;
    }
}

void UPX4Component::CleanupConnection()
{
    UE_LOG(LogPX4, Warning, TEXT("Cleaning up MavLinkCom connection"));
    
    try
    {
        if (MavLinkConn && MavLinkConn->isOpen())
        {
            MavLinkConn->close();
            UE_LOG(LogPX4, Warning, TEXT("MavLinkCom connection closed"));
        }
        
        MavLinkConn.reset();
        Vehicle.reset();
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogPX4, Error, TEXT("Exception in CleanupConnection: %s"), UTF8_TO_TCHAR(e.what()));
    }
    
    bConnected = false;
    bConnectedToPX4 = false;
}

// Old socket functions removed - now using MavLinkCom







void UPX4Component::SendHeartbeat()
{
    if (!MavLinkConn || !MavLinkConn->isOpen())
    {
        return;
    }
    
    try
    {
        mavlinkcom::MavLinkHeartbeat heartbeat;
        heartbeat.autopilot = MAV_AUTOPILOT_GENERIC;
        heartbeat.type = MAV_TYPE_QUADROTOR;
        heartbeat.system_status = MAV_STATE_ACTIVE;
        heartbeat.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
        heartbeat.custom_mode = 0;
        heartbeat.mavlink_version = 3;
        
        MavLinkConn->sendMessage(heartbeat);
        
        static int HeartbeatCount = 0;
        if (++HeartbeatCount % 10 == 0) // Log every 10th heartbeat
        {
            UE_LOG(LogPX4, Warning, TEXT("Sent heartbeat #%d to PX4"), HeartbeatCount);
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogPX4, Error, TEXT("Exception in SendHeartbeat: %s"), UTF8_TO_TCHAR(e.what()));
    }
}

void UPX4Component::SendHILStateQuaternion()
{
    // MavLinkCom state quaternion sending temporarily disabled
    static int StateCount = 0;
    if (++StateCount % 50 == 0)
    {
        UE_LOG(LogPX4, Warning, TEXT("HIL_STATE_QUATERNION #%d (stub) - MavLinkCom pending"), StateCount);
    }
}

void UPX4Component::SendHILSensor()
{
    if (!MavLinkConn || !MavLinkConn->isOpen())
    {
        return;
    }
    
    try
    {
        mavlinkcom::MavLinkHilSensor hilSensor;
        
        // Set timestamp
        hilSensor.time_usec = HILTimestamp;
        
        // Convert Unreal coordinates to PX4/NED
        float px, py, pz, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate;
        ConvertUnrealToPX4Coordinates(CurrentPosition, CurrentVelocity, CurrentRotation, CurrentAngularVelocity,
                                     px, py, pz, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate);
        
        // Accelerometer (m/s²) - derived from velocity changes or use IMU if available
        // For now, use gravity + estimated linear acceleration from velocity
        hilSensor.xacc = 0.0f; // Would calculate from velocity changes
        hilSensor.yacc = 0.0f;
        hilSensor.zacc = 9.81f; // Gravity component
        
        // Gyroscope (rad/s) - convert from deg/s
        hilSensor.xgyro = FMath::DegreesToRadians(rollRate);
        hilSensor.ygyro = FMath::DegreesToRadians(pitchRate);
        hilSensor.zgyro = FMath::DegreesToRadians(yawRate);
        
        // Magnetometer (Tesla) - default north-pointing values
        hilSensor.xmag = 0.000021f; // ~21 microteslas (typical Earth field)
        hilSensor.ymag = 0.0f;
        hilSensor.zmag = -0.000042f; // Downward component
        
        // Barometer - convert position to altitude and pressure
        float altitudeMeters = -pz; // Z is down in NED, so negate for altitude
        hilSensor.abs_pressure = 1013.25f - (altitudeMeters * 0.12f); // Rough pressure altitude
        hilSensor.diff_pressure = 0.0f; // No airspeed sensor
        hilSensor.pressure_alt = altitudeMeters;
        hilSensor.temperature = 20.0f; // 20°C
        
        // Fields updated
        hilSensor.fields_updated = 0x1FFF; // All sensor fields
        
        MavLinkConn->sendMessage(hilSensor);
        
        static int32 SensorCount = 0;
        SensorCount++;
        
        if (SensorCount <= 10 || SensorCount % 100 == 0)
        {
            UE_LOG(LogPX4, Warning, TEXT("Sent HIL_SENSOR #%d (ts=%llu): acc(%.2f,%.2f,%.2f), gyro(%.3f,%.3f,%.3f)"),
                   SensorCount, HILTimestamp, hilSensor.xacc, hilSensor.yacc, hilSensor.zacc,
                   hilSensor.xgyro, hilSensor.ygyro, hilSensor.zgyro);
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogPX4, Error, TEXT("Exception in SendHILSensor: %s"), UTF8_TO_TCHAR(e.what()));
    }
}

void UPX4Component::SendHILGPS()
{
    // GPS functionality temporarily disabled during MavLinkCom transition
    // TODO: Implement proper MavLinkCom GPS message
}
void UPX4Component::SendHILRCInputs()
{
    // RC inputs functionality temporarily disabled during MavLinkCom transition
    // TODO: Implement proper MavLinkCom RC inputs message
}



UQuadDroneController* UPX4Component::FindQuadController()
{
    UE_LOG(LogPX4, Warning, TEXT("Searching for QuadDroneController..."));
    
    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        UE_LOG(LogPX4, Warning, TEXT("Found QuadPawn owner: %s"), *QuadPawn->GetName());
        
        if (QuadPawn->QuadController)
        {
            UE_LOG(LogPX4, Warning, TEXT("Found QuadController via QuadPawn->QuadController: %p"), QuadPawn->QuadController);
            return QuadPawn->QuadController;
        }
        else
        {
            UE_LOG(LogPX4, Warning, TEXT("QuadPawn->QuadController is null - probably not initialized yet"));
        }
    }
    else
    {
        UE_LOG(LogPX4, Error, TEXT("Owner is not AQuadPawn! Owner: %s (Class: %s)"), 
               GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"),
               GetOwner() ? *GetOwner()->GetClass()->GetName() : TEXT("NULL"));
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

void UPX4Component::SendBasicHILData()
{
    // Send basic HIL data with default/static values when QuadController is not available
    // This allows PX4 to initialize and get out of UNINIT state
    
    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        // Get basic state directly from the pawn
        CurrentPosition = QuadPawn->GetActorLocation();
        CurrentVelocity = QuadPawn->GetVelocity();
        CurrentRotation = QuadPawn->GetActorRotation();
        CurrentAngularVelocity = FVector::ZeroVector; // Default to zero if no controller
        
        UE_LOG(LogPX4, Warning, TEXT("Sending basic HIL data without QuadController"));
        UE_LOG(LogPX4, Warning, TEXT("Position: %s, Rotation: %s"), 
               *CurrentPosition.ToString(), *CurrentRotation.ToString());
    }
    else
    {
        // Use completely default values if no pawn
        CurrentPosition = FVector::ZeroVector;
        CurrentVelocity = FVector::ZeroVector;
        CurrentRotation = FRotator::ZeroRotator;
        CurrentAngularVelocity = FVector::ZeroVector;
        
        UE_LOG(LogPX4, Warning, TEXT("Sending default HIL data (no QuadPawn)"));
    }
    
    // Update timestamp
    HILTimestamp += HIL_INTERVAL_US;
    
    // Send the HIL messages with available data
    SendHILSensor();
    SendHILStateQuaternion();
    SendHILGPS();
    SendHILRCInputs();
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
}


void UPX4Component::SendHILDataFromThread()
{
    // Check if we should send data (lockstep mode)
    if (!ShouldSendHILData()) 
    {
        return;
    }
    
    FScopeLock Lock(&StateMutex);
    
    if (!bThreadSafeDataValid) return;
    
    // Update current state from thread-safe copies
    CurrentPosition = ThreadSafePosition;
    CurrentVelocity = ThreadSafeVelocity;
    CurrentRotation = ThreadSafeRotation;
    CurrentAngularVelocity = ThreadSafeAngularVelocity;
    
    // Update timestamp
    HILTimestamp += HIL_INTERVAL_US;
    
    // Store the timestamp we're sending
    {
        FScopeLock LockstepLock(&LockstepMutex);
        LastSentHILTimestamp = HILTimestamp;
    }
    
    // Send HIL messages in precise timing
    SendHILSensor();
    
    // Log every 100th sensor message
    static int32 SensorCount = 0;
    if (++SensorCount % 100 == 0)
    {
        UE_LOG(LogPX4, Warning, TEXT("Sent HIL_SENSOR #%d (ts=%llu): acc(%.2f,%.2f,%.2f), gyro(%.3f,%.3f,%.3f)"),
               SensorCount, HILTimestamp,
               CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z,
               CurrentAngularVelocity.X, CurrentAngularVelocity.Y, CurrentAngularVelocity.Z);
    }
    
    // Only send state quaternion if we have a controller
    if (QuadController || !ThreadSafeAngularVelocity.IsZero())
    {
        SendHILStateQuaternion();
    }
    
    // Send GPS and RC at lower rate (10Hz)
    static int32 GPSCounter = 0;
    if (++GPSCounter % 10 == 0)
    {
        SendHILGPS();
        SendHILRCInputs();
    }
}

bool UPX4Component::IsConnectedToPX4() 
{
    return bConnected;
}

void UPX4Component::OnMessageReceived(std::shared_ptr<mavlinkcom::MavLinkMessage> message)
{
    if (!message) return;
    
    try
    {
        switch (message->msgid)
        {
            case static_cast<uint8_t>(mavlinkcom::MavLinkMessageIds::MAVLINK_MSG_ID_HEARTBEAT):
            {
                auto heartbeat = std::make_shared<mavlinkcom::MavLinkHeartbeat>();
                heartbeat->decode(*message);
                HandleHeartbeat(heartbeat);
                break;
            }
            case static_cast<uint8_t>(mavlinkcom::MavLinkMessageIds::MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS):
            {
                auto actuator = std::make_shared<mavlinkcom::MavLinkHilActuatorControls>();
                actuator->decode(*message);
                HandleHILActuatorControls(actuator);
                break;
            }
            default:
            {
                // Log other message types at verbose level
                static int32 MessageCount = 0;
                if (++MessageCount % 100 == 0)
                {
                    UE_LOG(LogPX4, Verbose, TEXT("Received MAVLink message ID %d (#%d total)"), message->msgid, MessageCount);
                }
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogPX4, Error, TEXT("Exception in OnMessageReceived: %s"), UTF8_TO_TCHAR(e.what()));
    }
}

void UPX4Component::HandleHeartbeat(std::shared_ptr<mavlinkcom::MavLinkHeartbeat> heartbeat)
{
    if (!bConnectedToPX4)
    {
        bConnectedToPX4 = true;
        UE_LOG(LogPX4, Warning, TEXT("PX4 connection established - PX4 mode: %d, status: %d"), 
               heartbeat->base_mode, heartbeat->system_status);
        
        // Initialize HIL timestamp to start from 10000 microseconds like PX4 expects
        HILTimestamp = 10000;
        
        UE_LOG(LogPX4, Warning, TEXT("Starting HIL data stream at 100Hz - Initial timestamp: %llu"), HILTimestamp);
        
        // Try to find QuadController if we don't have it
        if (!QuadController)
        {
            QuadController = FindQuadController();
        }
        
        // Start the dedicated communication thread
        if (!CommunicationThread)
        {
            CommunicationThread = new FPX4CommunicationThread(this);
            CommunicationThread->StartThread();
            UE_LOG(LogPX4, Warning, TEXT("PX4 Communication thread started"));
        }
        
        // Send initial HIL data immediately
        UpdateThreadSafeState();
        UE_LOG(LogPX4, Warning, TEXT("Sending initial HIL data to initialize PX4"));
        
        if (QuadController)
        {
            // Send with full controller data
            if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
            {
                CurrentPosition = QuadPawn->GetActorLocation();
                CurrentVelocity = QuadPawn->GetVelocity();
                CurrentRotation = QuadPawn->GetActorRotation();
                CurrentAngularVelocity = QuadController->GetLocalAngularRateDeg();
                
                SendHILSensor();
            }
        }
        else
        {
            // Send basic data to get PX4 initialized
            UE_LOG(LogPX4, Warning, TEXT("Sending basic HIL data (QuadController not ready yet)"));
            SendBasicHILData();
        }
    }
    
    // Reset connection timeout when we receive heartbeat from PX4
    ConnectionTimeoutTimer = 0.0f;
    LastHeartbeatTime = GetWorld()->GetTimeSeconds();
}

void UPX4Component::HandleHILActuatorControls(std::shared_ptr<mavlinkcom::MavLinkHilActuatorControls> actuator)
{
    // Handle lockstep synchronization
    {
        FScopeLock Lock(&LockstepMutex);
        LastActuatorControlsTimestamp = actuator->time_usec;
        bReceivedActuatorControls = true;
        LastLockstepUpdateTime = FPlatformTime::Seconds();
    }
    
    // Handle lockstep
    HandleLockstep();
    
    // Convert actuator controls to motor commands
    TArray<float> MotorCommands;
    MotorCommands.SetNum(4);
    
    bool isarmed = (actuator->mode & 128) != 0;
    for (int32 i = 0; i < 4 && i < 16; i++)
    {
        if (isarmed)
        {
            // Clamp and normalize from [-1,1] to [0,1]
            float NormalizedValue = FMath::Clamp((actuator->controls[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
            MotorCommands[i] = NormalizedValue;
        }
        else
        {
            MotorCommands[i] = 0.0f;
        }
    }
    
    // Apply motor commands to QuadController
    if (QuadController)
    {
        QuadController->ApplyMotorCommands(MotorCommands);
    }
    
    UE_LOG(LogPX4, Verbose, TEXT("Received HIL_ACTUATOR_CONTROLS: timestamp=%llu, armed=%d, M1=%.3f, M2=%.3f, M3=%.3f, M4=%.3f"), 
           actuator->time_usec, isarmed,
           MotorCommands[0], MotorCommands[1], MotorCommands[2], MotorCommands[3]);
}


void UPX4Component::HandleLockstep()
{
    if (!bEnableLockstep) return;
    
    FScopeLock Lock(&LockstepMutex);
    
    // Activate lockstep after we've received a few actuator control messages
    // This gives PX4 time to initialize with free-running HIL data
    if (!bLockstepActive && bReceivedActuatorControls)
    {
        static int32 ActuatorControlCount = 0;
        ActuatorControlCount++;
        
        // Activate lockstep after receiving 50 actuator control messages (about 0.5 seconds)
        if (ActuatorControlCount >= 50)
        {
            bLockstepActive = true;
            UE_LOG(LogPX4, Warning, TEXT("Lockstep mode activated after %d actuator control messages"), ActuatorControlCount);
        }
    }
    
    // Check for lockstep timeout
    double CurrentTime = FPlatformTime::Seconds();
    if (bLockstepActive && (CurrentTime - LastLockstepUpdateTime) > 1.0)
    {
        ResetLockstep(TEXT("Timeout on HIL_ACTUATOR_CONTROLS message"));
    }
}

void UPX4Component::ResetLockstep(const FString& Reason)
{
    FScopeLock Lock(&LockstepMutex);
    
    if (bLockstepActive)
    {
        bLockstepActive = false;
        LockstepResetCount++;
        UE_LOG(LogPX4, Warning, TEXT("Lockstep reset #%d: %s"), LockstepResetCount, *Reason);
    }
    
    bReceivedActuatorControls = false;
}

bool UPX4Component::ShouldSendHILData()
{
    if (!bEnableLockstep) return true; // Always send if lockstep is disabled
    
    FScopeLock Lock(&LockstepMutex);
    
    // CRITICAL: Always send HIL data during initial connection phase
    // PX4 needs continuous HIL_SENSOR stream to stay alive
    if (!bLockstepActive)
    {
        return true; // Send freely until lockstep is active
    }
    
    // In active lockstep mode, only send if we received actuator controls
    if (bReceivedActuatorControls)
    {
        bReceivedActuatorControls = false; // Reset flag
        return true;
    }
    
    return false;
}


