#include "SimulationCore/Public/Core/SimulationManager.h"
#include "SimulationCore/Public/Core/TimeController.h"
#include "SimulationCore/Public/Interfaces/ISimulatable.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Interfaces/SimAggregate.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsSettings.h"

ASimulationManager::ASimulationManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics; // own sim time before physics
    Mode = ESimMode::Robotics;                  // default
}

void ASimulationManager::BeginPlay()
{
    Super::BeginPlay();

    // Time controller for Robotics/Lockstep
    TimeController = NewObject<UTimeController>(this, TEXT("TimeController"));
    // Default robotics dt = 0.004 s (250 Hz) already set in ctor

    // Enable sub-stepping (safer physics) and back up
    if (UPhysicsSettings* PS = UPhysicsSettings::Get())
    {
        OriginalMaxPhysicsStep = PS->MaxPhysicsDeltaTime;
        OriginalSubstepping   = PS->bSubstepping;

        PS->bSubstepping        = true;
        PS->MaxSubstepDeltaTime = 0.004f; // match FixedTimestep
        PS->MaxSubsteps         = 8;      // protects 30 FPS budget
    }

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USimulatable::StaticClass(), Found);

    RegisteredRobots.Empty();
    for (AActor* A : Found)
    {
        if (!A) continue;
        // Only accept actors that are also marked as sim aggregates
        if (A->GetClass()->ImplementsInterface(USimAggregate::StaticClass()))
        {
            RegisteredRobots.Add(A);
        }
        else
        {
            // Ignore pawns or leaf nodes even if someone accidentally gave them ISimulatable
            UE_LOG(LogTemp, Verbose, TEXT("Ignored non-aggregate simulatable: %s"), *A->GetName());
        }
    }
    UE_LOG(LogTemp, Display, TEXT("SimulationManager: registered %d sim aggregates"), RegisteredRobots.Num());

    // Apply initial mode policy
    SetMode(Mode);
}

void ASimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UPhysicsSettings* PS = UPhysicsSettings::Get())
    {
        PS->MaxPhysicsDeltaTime = OriginalMaxPhysicsStep;
        PS->bSubstepping        = OriginalSubstepping;
    }

    RegisteredRobots.Empty();
    Super::EndPlay(EndPlayReason);
}

void ASimulationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (Mode)
    {
    case ESimMode::Cinematic: Tick_Cinematic(DeltaTime); break;
    case ESimMode::Robotics:  Tick_Robotics(DeltaTime);  break;
    case ESimMode::Lockstep:  Tick_Lockstep(DeltaTime);  break;
    case ESimMode::Paused:    /* idle */ if (bStepRequested) { ExecuteFixedStep(TimeController->GetFixedDeltaTime()); bStepRequested = false; } break;
    }
}

void ASimulationManager::Tick_Cinematic(float /*DeltaTime*/)
{
    // In Cinematic, Unreal’s world clock is scaled.
    // We still integrate physics via the engine; no manual fixed stepping.
    // (Nothing to do here; visual time dilation already applied in SetMode/SetCinematicTimeScale.)
}

void ASimulationManager::Tick_Robotics(float DeltaTime)
{
    if (!TimeController) return;

    // Accumulate scaled wall time (SimSpeed is applied inside TimeController)
    TimeController->AccumulateTime(DeltaTime);

    int32 Steps = 0;
    const int32 Budget = MaxStepsPerFrame;

    while (TimeController->ShouldStep() && Steps < Budget)
    {
        ExecuteFixedStep(TimeController->GetFixedDeltaTime());
        TimeController->ConsumeOneStep();
        ++Steps;
    }

    if (TimeController->ShouldStep() && Steps >= Budget)
    {
        UE_LOG(LogTemp, Verbose, TEXT("Robotics: hit per-frame cap %d; backlog=%.4f s"),
               Budget, TimeController->GetAccumulator());
        ShedLoadIfNeeded();
    }
}

void ASimulationManager::Tick_Lockstep(float /*DeltaTime*/)
{
    // In lockstep we only step on explicit external command (e.g., PX4 handshake)
    if (!bWaitingForExternalCommand)
    {
        ExecuteFixedStep(TimeController ? TimeController->GetFixedDeltaTime() : 0.004f);
        bWaitingForExternalCommand = true; // wait until RequestSimulationStep()
    }
}

void ASimulationManager::ExecuteFixedStep(float FixedDeltaTime)
{
    if (FixedDeltaTime <= 0.f) return;

    SimTimeSeconds += FixedDeltaTime;
    ++CurrentStep;

    // Integrate physics & drive all robots/sensors in fixed dt
    UpdateAllRobots(FixedDeltaTime);

    // You can publish /clock here (ROS 2), stamp with SimTimeSeconds
    // e.g., RosClockPublisher->Publish(SimTimeSeconds);
}

void ASimulationManager::UpdateAllRobots(float FixedDeltaTime)
{
    for (AActor* Robot : RegisteredRobots)
    {
        if (!Robot) continue;
        if (Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
        {
            ISimulatable::Execute_SimulationUpdate(Robot, FixedDeltaTime);
        }
    }
}

void ASimulationManager::SetMode(ESimMode NewMode)
{
    if (Mode == NewMode) return;

    // Reset world dilation unless Cinematic
    if (AWorldSettings* WS = GetWorld() ? GetWorld()->GetWorldSettings() : nullptr)
    {
        WS->SetTimeDilation(1.0f);
    }

    Mode = NewMode;
    bWaitingForExternalCommand = false;
    if (TimeController) TimeController->Reset();

    switch (Mode)
    {
    case ESimMode::Cinematic:
        if (AWorldSettings* WS = GetWorld() ? GetWorld()->GetWorldSettings() : nullptr)
            WS->SetTimeDilation(FMath::Clamp(CinematicTimeScale, 0.01f, 100.f));
        break;
    case ESimMode::Robotics:
        // Fixed-dt steppable; engine time remains 1.0
        if (TimeController) TimeController->SetPaused(false);
        break;
    case ESimMode::Lockstep:
        // One fixed step per external handshake; engine time at 1.0
        if (TimeController) TimeController->SetPaused(false);
        break;
    case ESimMode::Paused:
        if (TimeController) TimeController->SetPaused(true);
        break;
    }

    UE_LOG(LogTemp, Display, TEXT("Mode switched to %s"), *UEnum::GetValueAsString(Mode));
}

void ASimulationManager::SetSimSpeed(float NewSpeed)
{
    if (!TimeController) return;
    if (Mode != ESimMode::Robotics) return; // no-op outside Robotics
    TimeController->SetSimSpeed(NewSpeed);
}

float ASimulationManager::GetSimSpeed() const
{
    return TimeController ? TimeController->GetSimSpeed() : 1.0f;
}

void ASimulationManager::SetCinematicTimeScale(float NewScale)
{
    CinematicTimeScale = FMath::Clamp(NewScale, 0.01f, 100.f);
    if (Mode == ESimMode::Cinematic)
    {
        if (AWorldSettings* WS = GetWorld() ? GetWorld()->GetWorldSettings() : nullptr)
            WS->SetTimeDilation(CinematicTimeScale);
    }
}

void ASimulationManager::Pause()
{
    SetMode(ESimMode::Paused);
}

void ASimulationManager::Resume()
{
    // Resume to Robotics by default (you can make this configurable)
    SetMode(ESimMode::Robotics);
}

void ASimulationManager::StepOnce()
{
    if (Mode == ESimMode::Robotics)
    {
        ExecuteFixedStep(TimeController ? TimeController->GetFixedDeltaTime() : 0.004f);
    }
    else if (Mode == ESimMode::Lockstep)
    {
        // External step request (e.g., PX4) clears wait flag
        bWaitingForExternalCommand = false;
    }
    else if (Mode == ESimMode::Paused)
    {
        bStepRequested = true;
    }
}

void ASimulationManager::ResetSimulation()
{
    SimTimeSeconds = 0.0;
    CurrentStep = 0;
    if (TimeController) TimeController->Reset();

    for (AActor* Robot : RegisteredRobots)
    {
        if (Robot && Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
        {
            ISimulatable::Execute_ResetRobot(Robot);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("Simulation reset"));
}

void ASimulationManager::RegisterRobot(AActor* Robot)
{
    if (!Robot || RegisteredRobots.Contains(Robot)) return;

    if (Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
    {
        RegisteredRobots.Add(Robot);
        UE_LOG(LogTemp, Display, TEXT("Registered robot: %s"), *Robot->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor %s does not implement ISimulatable"), *Robot->GetName());
    }
}

void ASimulationManager::UnregisterRobot(AActor* Robot)
{
    if (!Robot) return;
    RegisteredRobots.Remove(Robot);
    UE_LOG(LogTemp, Display, TEXT("Unregistered robot: %s"), *Robot->GetName());
}

ASimulationManager* ASimulationManager::Get(UWorld* World)
{
    if (!World) return nullptr;
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, ASimulationManager::StaticClass(), Found);
    return Found.Num() > 0 ? Cast<ASimulationManager>(Found[0]) : nullptr;
}

void ASimulationManager::ShedLoadIfNeeded()
{
    // Stub: this is where you’d throttle cameras/lidar, reduce resolution,
    // or clamp SimSpeed slightly when FPS dips under MinTargetFPS.
    // You can sample GEngine->GetAverageFPS() or measure step costs yourself.
}
