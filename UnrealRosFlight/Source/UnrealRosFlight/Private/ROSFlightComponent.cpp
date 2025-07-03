// ROSFlightComponent.cpp

#include "ROSFlightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Msgs/ROS2WrenchStamped.h"
#include "Msgs/ROS2Imu.h"
#include "Msgs/ROS2SimState.h"
#include "Msgs/ROS2Command.h"
#include "Msgs/ROS2MagneticField.h"
#include "Msgs/ROS2Barometer.h"
#include "Msgs/ROS2BatteryStatus.h"
#include "Srvs/ROS2StdSrvSetBool.h"
#include "Common/UdpSocketBuilder.h"
#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSFlight, Log, All);

UROSFlightComponent::UROSFlightComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;
    RandStream.Initialize(FMath::Rand());
}

void UROSFlightComponent::SetControllerSource(UObject* InControllerSource)
{
    ControllerSource.SetObject(InControllerSource);
}

void UROSFlightComponent::BeginPlay()
{
    Super::BeginPlay();

    // Setup UDP Socket for MAVLink
    FIPv4Address IPAddress;
    FIPv4Address::Parse(HostAddress, IPAddress);

    RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    RemoteAddr->SetIp(IPAddress.Value);
    RemoteAddr->SetPort(Port);

    Socket = FUdpSocketBuilder(TEXT("ROSFlightSocket"))
        .AsNonBlocking()
        .AsReusable()
        .BoundToPort(Port);

    if (Socket)
    {
        UE_LOG(LogROSFlight, Warning, TEXT("UDP socket created and bound to port %d"), Port);
    }
    else
    {
        UE_LOG(LogROSFlight, Error, TEXT("Failed to create UDP socket on port %d!"), Port);
    }

    // Get Physics Body
    Body = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    if (!Body || !Body->IsSimulatingPhysics())
    {
        UE_LOG(LogROSFlight, Error, TEXT("ROSFlightComponent: root must simulate physics!"));
        return;
    }

    // ROS Node setup
    Node = GetOwner()->FindComponentByClass<UROS2NodeComponent>();
    if (!Node)
    {
        Node = NewObject<UROS2NodeComponent>(GetOwner(), TEXT("ROSFlightNode"));
        Node->RegisterComponent();
    }
    Node->Name = TEXT("unreal_rosflight");
    Node->Namespace = TEXT("");
    Node->Init();

    // Create Publishers
    SimStatePub = Node->CreatePublisher(SimStateTopic, UROS2Publisher::StaticClass(), UROS2SimStateMsg::StaticClass(), UROS2QoS::Default);
    ImuPub = Node->CreatePublisher(ImuTopic, UROS2Publisher::StaticClass(), UROS2ImuMsg::StaticClass(), UROS2QoS::SensorData);
    CommandPub = Node->CreatePublisher(CommandTopic, UROS2Publisher::StaticClass(), UROS2CommandMsg::StaticClass(), UROS2QoS::Default);
    
    // Optional sensors
    MagPub = Node->CreatePublisher(MagTopic, UROS2Publisher::StaticClass(), UROS2MagneticFieldMsg::StaticClass(), UROS2QoS::Default);
    BaroPub = Node->CreatePublisher(BaroTopic, UROS2Publisher::StaticClass(), UROS2BarometerMsg::StaticClass(), UROS2QoS::Default);
    BatteryPub = Node->CreatePublisher(BatteryTopic, UROS2Publisher::StaticClass(), UROS2BatteryStatusMsg::StaticClass(), UROS2QoS::Default);

    // Create Subscribers and Services
    ROS2_CREATE_SUBSCRIBER(Node, this, WrenchTopic, UROS2WrenchStampedMsg::StaticClass(), &UROSFlightComponent::HandleWrenchMsg);
    ROS2_CREATE_SERVICE_SERVER(Node, this, TEXT("/sil_board/run"), UROS2StdSrvSetBoolSrv::StaticClass(), &UROSFlightComponent::HandleRunService);

    UE_LOG(LogROSFlight, Warning, TEXT("ROSFlightComponent initialized successfully"));
}

void UROSFlightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
    
    Super::EndPlay(EndPlayReason);
}

void UROSFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!Body || !Node) return;

    // Handle UDP data
    ReceiveData();
    
    // Publish at different rates
    static int TickCounter = 0;
    TickCounter++;

    // High frequency (every tick)
    if (TickCounter % 1 == 0)
    {
        PublishIMU();
        PublishTruthState();
        
        // Publish commands when ROSflight mode is active
        if (ControllerSource && ControllerSource->IsUsingROSflight())
        {
            PublishCommands();
        }
    }

    // Lower frequency sensors (every 6 ticks)
    if (TickCounter % 6 == 0)
    {
        PublishMinimalSensors();
    }
}

void UROSFlightComponent::PublishIMU()
{
    if (!ImuPub || !Body) return;

    const float Dt = GetWorld()->GetDeltaSeconds();
    
    // Get current state
    const FQuat WorldQuat = GetOwner()->GetActorQuat();
    const FVector WorldLinVel = Body->GetPhysicsLinearVelocity();
    const FVector WorldAngVel = Body->GetPhysicsAngularVelocityInRadians();

    // Convert to ROS coordinate frame
    const FQuat RosQuat = UnrealToRos(WorldQuat);
    const FVector RosAngVelBody = UnrealToRos(WorldQuat.UnrotateVector(WorldAngVel));

    // Compute acceleration
    FVector WorldAccel = FVector::ZeroVector;
    if (!bFirstSample) 
    {
        WorldAccel = (WorldLinVel - PrevLinVel) / Dt;
    }
    PrevLinVel = WorldLinVel;
    bFirstSample = false;

    const FVector RosLinAccelBody = UnrealToRos(WorldQuat.UnrotateVector(WorldAccel));

    // Add bias and noise
    StepGaussMarkov(BiasGyro, NoiseSigmaGyro, Dt);
    StepGaussMarkov(BiasAccel, NoiseSigmaAccel, Dt);

    const FVector GyroMeas = RosAngVelBody + BiasGyro + FVector(
        NoiseSigmaGyro.X * SampleNormal(),
        NoiseSigmaGyro.Y * SampleNormal(),
        NoiseSigmaGyro.Z * SampleNormal()
    );

    const FVector AccelMeas = RosLinAccelBody + BiasAccel + FVector(
        NoiseSigmaAccel.X * SampleNormal(),
        NoiseSigmaAccel.Y * SampleNormal(),
        NoiseSigmaAccel.Z * SampleNormal()
    );

    // Create and publish IMU message
    if (auto* ImuMsg = Cast<UROS2ImuMsg>(ImuPub->TopicMessage))
    {
        FROSImu ImuData;
        ImuData.Header.FrameId = TEXT("base_link");
        
        // Set orientation
        ImuData.Orientation.X = RosQuat.X;
        ImuData.Orientation.Y = RosQuat.Y;
        ImuData.Orientation.Z = RosQuat.Z;
        ImuData.Orientation.W = RosQuat.W;
        
        // Set angular velocity
        ImuData.AngularVelocity.X = GyroMeas.X;
        ImuData.AngularVelocity.Y = GyroMeas.Y;
        ImuData.AngularVelocity.Z = GyroMeas.Z;
        
        // Set linear acceleration
        ImuData.LinearAcceleration.X = AccelMeas.X;
        ImuData.LinearAcceleration.Y = AccelMeas.Y;
        ImuData.LinearAcceleration.Z = AccelMeas.Z;
        
        ImuMsg->SetMsg(ImuData);
        ImuPub->Publish();
    }
}

void UROSFlightComponent::PublishTruthState()
{
    if (!SimStatePub || !Body) return;

    // Get current aircraft state
    const FVector Position = GetOwner()->GetActorLocation();
    const FQuat Orientation = GetOwner()->GetActorQuat();
    const FVector LinearVel = Body->GetPhysicsLinearVelocity();
    const FVector AngularVel = Body->GetPhysicsAngularVelocityInRadians();

    // Convert to ROS coordinates (including cm to m conversion)
    const FVector RosPosition = UnrealToRos(Position) / 100.0f;
    const FQuat RosOrientation = UnrealToRos(Orientation);
    const FVector RosLinearVel = UnrealToRos(LinearVel) / 100.0f;
    const FVector RosAngularVel = UnrealToRos(AngularVel);

    // Create and publish truth state message
    if (auto* TruthMsg = Cast<UROS2SimStateMsg>(SimStatePub->TopicMessage))
    {
        FROSSimState TruthData;
        TruthData.Header.FrameId = TEXT("world");
        
        // Set pose
        TruthData.Pose.Position.X = RosPosition.X;
        TruthData.Pose.Position.Y = RosPosition.Y;
        TruthData.Pose.Position.Z = RosPosition.Z;
        
        TruthData.Pose.Orientation.X = RosOrientation.X;
        TruthData.Pose.Orientation.Y = RosOrientation.Y;
        TruthData.Pose.Orientation.Z = RosOrientation.Z;
        TruthData.Pose.Orientation.W = RosOrientation.W;
        
        // Set twist (in body frame)
        const FVector BodyLinearVel = Orientation.UnrotateVector(LinearVel) / 100.0f;
        const FVector BodyAngularVel = Orientation.UnrotateVector(AngularVel);
        
        TruthData.Twist.Linear.X = UnrealToRos(BodyLinearVel).X;
        TruthData.Twist.Linear.Y = UnrealToRos(BodyLinearVel).Y;
        TruthData.Twist.Linear.Z = UnrealToRos(BodyLinearVel).Z;
        
        TruthData.Twist.Angular.X = UnrealToRos(BodyAngularVel).X;
        TruthData.Twist.Angular.Y = UnrealToRos(BodyAngularVel).Y;
        TruthData.Twist.Angular.Z = UnrealToRos(BodyAngularVel).Z;
        
        TruthMsg->SetMsg(TruthData);
        SimStatePub->Publish();
    }
}

void UROSFlightComponent::PublishMinimalSensors()
{
    // Magnetometer
    if (MagPub)
    {
        if (auto* MagMsg = Cast<UROS2MagneticFieldMsg>(MagPub->TopicMessage))
        {
            FROSMagneticField MagData;
            MagData.Header.FrameId = TEXT("base_link");
            
            // Simple magnetic field model (points north with inclination)
            const float Declination = 0.0f; // Magnetic declination
            const float Inclination = 1.0f; // Magnetic inclination
            
            // Earth's magnetic field in NED frame
            FVector MagFieldNED(0.2f * FMath::Cos(Inclination), 
                               0.0f, 
                               0.2f * FMath::Sin(Inclination));
            
            // Rotate to body frame
            const FQuat WorldQuat = GetOwner()->GetActorQuat();
            const FVector MagFieldBody = WorldQuat.UnrotateVector(MagFieldNED);
            const FVector RosMagField = UnrealToRos(MagFieldBody);
            
            MagData.MagneticField.X = RosMagField.X;
            MagData.MagneticField.Y = RosMagField.Y;
            MagData.MagneticField.Z = RosMagField.Z;
            
            MagMsg->SetMsg(MagData);
            MagPub->Publish();
        }
    }

    // Barometer
    if (BaroPub)
    {
        if (auto* BaroMsg = Cast<UROS2BarometerMsg>(BaroPub->TopicMessage))
        {
            FROSBarometer BaroData;
            BaroData.Header.FrameId = TEXT("base_link");
            
            const float Altitude = GetOwner()->GetActorLocation().Z / 100.0f; // Convert to meters
            const float SeaLevelPressure = 101325.0f; // Pa
            const float Temperature = 15.0f; // Celsius
            
            // Barometric formula
            BaroData.Pressure = SeaLevelPressure * FMath::Pow(1.0f - (0.0065f * Altitude) / (Temperature + 273.15f), 5.255f);
            BaroData.Temperature = Temperature;
            
            BaroMsg->SetMsg(BaroData);
            BaroPub->Publish();
        }
    }

    // Battery
    if (BatteryPub)
    {
        if (auto* BattMsg = Cast<UROS2BatteryStatusMsg>(BatteryPub->TopicMessage))
        {
            FROSBatteryStatus BattData;
            
            // Simple battery simulation
            static float FlightTime = 0.0f;
            FlightTime += GetWorld()->GetDeltaSeconds();
            
            // Simulate battery drain over 20 minutes
            const float MaxFlightTime = 1200.0f; // 20 minutes
            const float BatteryLevel = FMath::Max(0.0f, 1.0f - (FlightTime / MaxFlightTime));
            
            BattData.Voltage = BatteryVoltage * (0.8f + 0.2f * BatteryLevel); // Voltage drops with discharge
            BattData.Current = BatteryCurrentDraw * (ControllerSource && ControllerSource->IsUsingROSflight() ? 1.0f : 0.1f);

            
            BattMsg->SetMsg(BattData);
            BatteryPub->Publish();
        }
    }
}

void UROSFlightComponent::PublishCommands()
{
    if (!CommandPub || !ControllerSource) return;

    // Only publish commands when ROSflight mode is active
    if (!ControllerSource->IsUsingROSflight()) return;

    // Get commands from the controller interface
    const float Roll = ControllerSource->GetDesiredRoll();
    const float Pitch = ControllerSource->GetDesiredPitch();
    const float YawRate = ControllerSource->GetDesiredYawRate();
    const float Thrust = ControllerSource->GetDesiredThrustNormalized();

    // Create and publish command message
    if (auto* CmdMsg = Cast<UROS2CommandMsg>(CommandPub->TopicMessage))
    {
        FROSCommand Command;
        Command.Header.FrameId = TEXT("base_link");
        Command.Mode = 2; // MODE_ROLL_PITCH_YAWRATE_THROTTLE
        Command.Ignore = 0; // IGNORE_NONE
        
        // Convert degrees to radians
        Command.Qx = FMath::DegreesToRadians(Roll);
        Command.Qy= FMath::DegreesToRadians(Pitch);
        Command.Qz = FMath::DegreesToRadians(YawRate);
        Command.Fx = Thrust;
        
        CmdMsg->SetMsg(Command);
        CommandPub->Publish();
        
        UE_LOG(LogROSFlight, VeryVerbose, TEXT("Published command: Roll=%.1f, Pitch=%.1f, YawRate=%.1f, Thrust=%.2f"), 
               Roll, Pitch, YawRate, Thrust);
    }
}

void UROSFlightComponent::HandleWrenchMsg(const UROS2GenericMsg* InMsg)
{
    const auto* WrenchMsg = Cast<UROS2WrenchStampedMsg>(InMsg);
    if (!WrenchMsg || !Body) return;

    FROSWrenchStamped WrenchData;
    WrenchMsg->GetMsg(WrenchData);

    // Convert forces and torques from ROS to Unreal coordinates
    const FVector ForceBody = FVector(
        WrenchData.Wrench.Force.X,
        WrenchData.Wrench.Force.Y,
        WrenchData.Wrench.Force.Z
    );
    const FVector TorqueBody = FVector(
        WrenchData.Wrench.Torque.X,
        WrenchData.Wrench.Torque.Y,
        WrenchData.Wrench.Torque.Z
    );

    // Apply forces and torques in world frame
    const FQuat WorldQuat = GetOwner()->GetActorQuat();
    Body->AddForce(WorldQuat.RotateVector(RosToUnreal(ForceBody)) * 100.0f, NAME_None, true); // Convert N to cm
    Body->AddTorqueInRadians(WorldQuat.RotateVector(RosToUnreal(TorqueBody)), NAME_None, true);
    
    UE_LOG(LogROSFlight, VeryVerbose, TEXT("Applied forces: F=(%.1f,%.1f,%.1f), T=(%.1f,%.1f,%.1f)"), 
           ForceBody.X, ForceBody.Y, ForceBody.Z, TorqueBody.X, TorqueBody.Y, TorqueBody.Z);
}

void UROSFlightComponent::HandleRunService(UROS2GenericSrv* InSrv)
{
    if (auto* Service = Cast<UROS2StdSrvSetBoolSrv>(InSrv))
    {
        FROSStdSrvSetBoolRes Response;
        Response.bSuccess = true;
        Response.Message = TEXT("Unreal runs real-time");
        Service->SetResponse(Response);
    }
}

double UROSFlightComponent::SampleNormal()
{
    const double U1 = FMath::Clamp(RandStream.GetFraction(), SMALL_NUMBER, 1.0 - SMALL_NUMBER);
    const double U2 = RandStream.GetFraction();
    return FMath::Sqrt(-2.0 * FMath::Loge(U1)) * FMath::Cos(2.0 * PI * U2);
}

void UROSFlightComponent::StepGaussMarkov(FVector& Bias, const FVector& Sigma, float Dt)
{
    const float Alpha = FMath::Exp(-Dt / BiasTau);
    const float Beta = FMath::Sqrt(1.0f - Alpha * Alpha);
    
    Bias.X = Alpha * Bias.X + Beta * Sigma.X * SampleNormal();
    Bias.Y = Alpha * Bias.Y + Beta * Sigma.Y * SampleNormal();
    Bias.Z = Alpha * Bias.Z + Beta * Sigma.Z * SampleNormal();
}

FVector UROSFlightComponent::UnrealToRos(const FVector& UnrealVec)
{
    // Unreal: X=Forward, Y=Right, Z=Up
    // ROS/NED: X=North(Forward), Y=East(Left), Z=Down
    return FVector(UnrealVec.X, -UnrealVec.Y, -UnrealVec.Z);
}

FVector UROSFlightComponent::RosToUnreal(const FVector& RosVec)
{
    // ROS/NED: X=North(Forward), Y=East(Left), Z=Down
    // Unreal: X=Forward, Y=Right, Z=Up
    return FVector(RosVec.X, -RosVec.Y, -RosVec.Z);
}

FQuat UROSFlightComponent::UnrealToRos(const FQuat& UnrealQuat)
{
    // Convert quaternion accounting for coordinate system differences
    // This handles the Y-axis flip and Z-axis inversion
    return FQuat(UnrealQuat.X, -UnrealQuat.Y, -UnrealQuat.Z, UnrealQuat.W);
}

void UROSFlightComponent::ReceiveData()
{
    if (!Socket) return;

    TArray<uint8> ReceivedData;
    uint32 Size;
    while (Socket->HasPendingData(Size))
    {
        ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
        int32 Read = 0;
        
        TSharedRef<FInternetAddr> SenderAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), Read, *SenderAddr);

        if (Read > 0)
        {
            // TODO: Parse MAVLink packets here
            UE_LOG(LogROSFlight, Verbose, TEXT("Received %d bytes from %s"), Read, *SenderAddr->ToString(true));
        }
    }
}