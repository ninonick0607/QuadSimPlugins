// QuadSimPlugin/Source/QuadSimCore/Private/Components/PX4Component.cpp
#include "Controllers/PX4Component.h"
#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "Engine/World.h"
#include <chrono>

// Include MAVLink after MAVSDK to avoid conflicts
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // Disable deprecation warnings for MAVLink
#endif
#include <common/mavlink.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

DEFINE_LOG_CATEGORY_STATIC(LogPX4, Log, All);

UPX4Component::UPX4Component()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

UPX4Component::~UPX4Component()
{
    DisconnectFromPX4();
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
    DisconnectFromPX4();
    Super::EndPlay(EndPlayReason);
}

void UPX4Component::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bUsePX4) return;

    // Update connection timeout
    if (bConnectedToPX4)
    {
        ConnectionTimeoutTimer += DeltaTime;
        if (ConnectionTimeoutTimer > ConnectionTimeout)
        {
            UE_LOG(LogPX4, Warning, TEXT("PX4 connection timeout - no heartbeat received"));
            DisconnectFromPX4();
        }
    }

    // Update state for MAVSDK thread
    if (bConnected)
    {
        UpdateStateFromGameThread();
    }
}

void UPX4Component::SetPX4Active(bool bActive)
{
    if (bActive && !bUsePX4)
    {
        bUsePX4 = true;
        ConnectToPX4();

        if (QuadController)
        {
            QuadController->SetUseExternalController(true);
        }
    }
    else if (!bActive && bUsePX4)
    {
        bUsePX4 = false;
        DisconnectFromPX4();

        if (QuadController)
        {
            QuadController->SetUseExternalController(false);
        }
    }
}

void UPX4Component::ConnectToPX4()
{
    if (bConnected) return;

    UE_LOG(LogPX4, Warning, TEXT("Connecting to PX4 via MAVSDK: %s"), *ConnectionString);

    // Initialize MAVSDK with proper configuration
    mavsdk::Mavsdk::Configuration config(mavsdk::ComponentType::GroundStation);
    MavSDK = std::make_unique<mavsdk::Mavsdk>(config);

    // Start connection
    std::string ConnStr = TCHAR_TO_UTF8(*ConnectionString);
    mavsdk::ConnectionResult result = MavSDK->add_any_connection(ConnStr);

    if (result != mavsdk::ConnectionResult::Success)
    {
        UE_LOG(LogPX4, Error, TEXT("Failed to connect"));
        return;
    }

    bConnected = true;
    bShouldStop = false;

    // Start MAVSDK thread
    MAVSDKThread = std::make_unique<std::thread>(&UPX4Component::RunMAVSDKThread, this);

    UE_LOG(LogPX4, Warning, TEXT("MAVSDK thread started, waiting for PX4..."));
}

void UPX4Component::DisconnectFromPX4()
{
    if (!bConnected) return;

    UE_LOG(LogPX4, Warning, TEXT("Disconnecting from PX4"));

    // Signal thread to stop
    bShouldStop = true;

    // Wait for thread to finish
    if (MAVSDKThread && MAVSDKThread->joinable())
    {
        MAVSDKThread->join();
        MAVSDKThread.reset();
    }

    // Clean up MAVSDK objects
    Passthrough.reset();
    Action.reset();
    Telemetry.reset();
    System.reset();
    MavSDK.reset();

    bConnected = false;
    bConnectedToPX4 = false;
}

void UPX4Component::RunMAVSDKThread()
{
    UE_LOG(LogPX4, Warning, TEXT("MAVSDK thread started"));

    // Wait for system discovery
    while (MavSDK->systems().size() == 0)
    {
        if (bShouldStop) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    System = MavSDK->systems().at(0);

    // Initialize plugins
    Telemetry = std::make_shared<mavsdk::Telemetry>(System);
    Action = std::make_shared<mavsdk::Action>(System);
    Passthrough = std::make_shared<mavsdk::MavlinkPassthrough>(System);

    UE_LOG(LogPX4, Warning, TEXT("Connected to PX4 system!"));
    bConnectedToPX4 = true;

    // Subscribe to incoming messages
    Passthrough->subscribe_message(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) {
            ConnectionTimeoutTimer = 0.0f; // Reset timeout
            LastHeartbeatTime = FPlatformTime::Seconds();
        });

    Passthrough->subscribe_message(
        MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS,
        [this](const mavlink_message_t& message) {
            mavlink_hil_actuator_controls_t actuator;
            mavlink_msg_hil_actuator_controls_decode(&message, &actuator);

            // Convert to motor commands
            TArray<float> MotorCommands;
            MotorCommands.SetNum(4);

            bool isArmed = (actuator.mode & 128) != 0;
            for (int32 i = 0; i < 4; i++)
            {
                if (isArmed)
                {
                    MotorCommands[i] = FMath::Clamp((actuator.controls[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
                }
                else
                {
                    MotorCommands[i] = 0.0f;
                }
            }

            // Apply to controller on game thread
            AsyncTask(ENamedThreads::GameThread, [this, MotorCommands]() {
                if (QuadController)
                {
                    QuadController->ApplyMotorCommands(MotorCommands);
                }
                });
        });

    // Main HIL loop - 250Hz for PX4 SITL
    auto last_time = std::chrono::steady_clock::now();
    const auto interval = std::chrono::microseconds(4000); // 250Hz

    while (!bShouldStop)
    {
        auto now = std::chrono::steady_clock::now();

        if (now - last_time >= interval)
        {
            SendHILSensor();
            last_time = now;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    UE_LOG(LogPX4, Warning, TEXT("MAVSDK thread stopped"));
}

void UPX4Component::SendHILSensor()
{
    if (!Passthrough) return;

    mavlink_hil_sensor_t hil_sensor = {};

    // Get current state (thread-safe)
    FVector Position, Velocity, AngularVelocity;
    FRotator Rotation;
    {
        FScopeLock Lock(&StateMutex);
        Position = CurrentPosition;
        Velocity = CurrentVelocity;
        Rotation = CurrentRotation;
        AngularVelocity = CurrentAngularVelocity;
    }

    // Convert to PX4 coordinates
    float px, py, pz, vx, vy, vz, q0, q1, q2, q3, rollRate, pitchRate, yawRate;
    ConvertUnrealToPX4Coordinates(Position, Velocity, Rotation, AngularVelocity,
        px, py, pz, vx, vy, vz, q0, q1, q2, q3,
        rollRate, pitchRate, yawRate);

    // Fill HIL sensor message
    hil_sensor.time_usec = HILTimestamp;

    // Accelerometer (m/s²) - gravity + any acceleration
    hil_sensor.xacc = 0.0f;
    hil_sensor.yacc = 0.0f;
    hil_sensor.zacc = 9.81f;

    // Gyroscope (rad/s)
    hil_sensor.xgyro = FMath::DegreesToRadians(rollRate);
    hil_sensor.ygyro = FMath::DegreesToRadians(pitchRate);
    hil_sensor.zgyro = FMath::DegreesToRadians(yawRate);

    // Magnetometer (Gauss)
    hil_sensor.xmag = 0.21f;
    hil_sensor.ymag = 0.0f;
    hil_sensor.zmag = 0.42f;

    // Barometer
    float altitudeMeters = -pz;
    hil_sensor.abs_pressure = 1013.25f * powf(1.0f - 0.0065f * altitudeMeters / 288.15f, 5.255f);
    hil_sensor.diff_pressure = 0.0f;
    hil_sensor.pressure_alt = altitudeMeters;
    hil_sensor.temperature = 20.0f;

    hil_sensor.fields_updated = 0x1FFF;

    // Send via passthrough
    mavlink_message_t message;
    mavlink_msg_hil_sensor_encode(1, 1, &message, &hil_sensor);
    Passthrough->send_message(message);

    // Update timestamp
    HILTimestamp += 4000; // 250Hz

    // Log periodically
    static int32 Count = 0;
    if (++Count % 250 == 0) // Every second
    {
        UE_LOG(LogPX4, Warning, TEXT("HIL_SENSOR sent #%d"), Count);
    }
}

void UPX4Component::UpdateStateFromGameThread()
{
    if (!QuadController)
    {
        QuadController = FindQuadController();
    }

    FScopeLock Lock(&StateMutex);

    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        CurrentPosition = QuadPawn->GetActorLocation();
        CurrentVelocity = QuadPawn->GetVelocity();
        CurrentRotation = QuadPawn->GetActorRotation();

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

UQuadDroneController* UPX4Component::FindQuadController()
{
    if (AQuadPawn* QuadPawn = Cast<AQuadPawn>(GetOwner()))
    {
        return QuadPawn->QuadController;
    }
    return nullptr;
}

void UPX4Component::ConvertUnrealToPX4Coordinates(const FVector& UnrealPos, const FVector& UnrealVel,
    const FRotator& UnrealRot, const FVector& UnrealAngVel,
    float& OutX, float& OutY, float& OutZ,
    float& OutVx, float& OutVy, float& OutVz,
    float& OutQ0, float& OutQ1, float& OutQ2, float& OutQ3,
    float& OutRollRate, float& OutPitchRate, float& OutYawRate)
{
    // Position: Convert cm to meters
    OutX = UnrealPos.X / 100.0f;
    OutY = UnrealPos.Y / 100.0f;
    OutZ = -UnrealPos.Z / 100.0f;

    // Velocity: Convert cm/s to m/s
    OutVx = UnrealVel.X / 100.0f;
    OutVy = UnrealVel.Y / 100.0f;
    OutVz = -UnrealVel.Z / 100.0f;

    // Rotation: Convert to quaternion
    FQuat UnrealQuat = FQuat(UnrealRot);
    OutQ0 = UnrealQuat.W;
    OutQ1 = UnrealQuat.X;
    OutQ2 = UnrealQuat.Y;
    OutQ3 = -UnrealQuat.Z;

    // Angular rates
    OutRollRate = UnrealAngVel.X;
    OutPitchRate = UnrealAngVel.Y;
    OutYawRate = -UnrealAngVel.Z;
}