#include "SimulationCore/Public/Core/SimulationManager.h"
#include "SimulationCore/Public/Core/TimeController.h"
#include "SimulationCore/Public/Interfaces/ISimulatable.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "imgui.h"

ASimulationManager::ASimulationManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;
    
    CurrentSimulationMode = ESimulationMode::Realtime;
    CurrentSimulationTime = 0.0f;
    CurrentEpisode = 0;
    CurrentStep = 0;
    bWaitingForExternalCommand = false;
    MaxStepsPerFrame = 10;
    bShowImGuiWindow = true;
    SelectedRobotIndex = 0;
}

void ASimulationManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Create Time Controller
    TimeController = NewObject<UTimeController>(this, TEXT("TimeController"));
    
    UE_LOG(LogTemp, Warning, TEXT("SimulationManager initialized with mode: %s"), 
           *UEnum::GetValueAsString(CurrentSimulationMode));
    
    // Find existing robots in the scene
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USimulatable::StaticClass(), FoundActors);
    
    for (AActor* Actor : FoundActors)
    {
        RegisterRobot(Actor);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Found and registered %d robots"), RegisteredRobots.Num());
}

void ASimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RegisteredRobots.Empty();
    Super::EndPlay(EndPlayReason);
}

void ASimulationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Draw ImGui window
    DrawImGuiWindow();
    
    // Handle different simulation modes
    switch (CurrentSimulationMode)
    {
    case ESimulationMode::Realtime:
        StepSimulation(DeltaTime);
        break;
        
    case ESimulationMode::FastForward:
        StepSimulation(DeltaTime * TimeController->GetTimeScale());
        break;
        
    case ESimulationMode::Lockstep:
        if (!bWaitingForExternalCommand)
        {
            StepSimulation(TimeController->GetFixedDeltaTime());
            bWaitingForExternalCommand = true;
        }
        break;
        
    case ESimulationMode::Paused:
        // Do nothing - simulation is paused
        break;
    }
}

void ASimulationManager::RegisterRobot(AActor* Robot)
{
    if (!Robot || RegisteredRobots.Contains(Robot))
    {
        return;
    }
    
    // Check if it implements ISimulatable
    if (Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
    {
        RegisteredRobots.Add(Robot);
        UE_LOG(LogTemp, Display, TEXT("Registered robot: %s"), *Robot->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor %s does not implement ISimulatable interface"), 
               *Robot->GetName());
    }
}

void ASimulationManager::UnregisterRobot(AActor* Robot)
{
    if (Robot)
    {
        RegisteredRobots.Remove(Robot);
        UE_LOG(LogTemp, Display, TEXT("Unregistered robot: %s"), *Robot->GetName());
    }
}

void ASimulationManager::StepSimulation(float DeltaTime)
{
    if (!TimeController)
    {
        return;
    }
    
    // Add time to accumulator
    TimeController->AccumulateTime(DeltaTime);
    
    // Execute fixed timesteps
    int32 StepsExecuted = 0;
    while (TimeController->ShouldStep() && StepsExecuted < MaxStepsPerFrame)
    {
        float FixedDeltaTime = TimeController->GetFixedDeltaTime();
        ExecuteSimulationStep(FixedDeltaTime);
        TimeController->ConsumeTime();
        StepsExecuted++;
    }
    
    // Warn if we're falling behind
    if (StepsExecuted >= MaxStepsPerFrame)
    {
        UE_LOG(LogTemp, Warning, TEXT("Simulation falling behind! Executed max steps (%d) this frame"), 
               MaxStepsPerFrame);
    }
}

void ASimulationManager::ExecuteSimulationStep(float FixedDeltaTime)
{
    // Update simulation time
    CurrentSimulationTime += FixedDeltaTime;
    CurrentStep++;
    
    // Update all robots with fixed timestep
    UpdateAllRobots(FixedDeltaTime);
    
    // // Step physics if in lockstep mode
    // if (CurrentSimulationMode == ESimulationMode::Lockstep)
    // {
    //     if (UWorld* World = GetWorld())
    //     {
    //         // This ensures physics runs for exactly one fixed timestep
    //         World->GetPhysicsScene()->StartFrame();
    //         World->GetPhysicsScene()->EndFrame();
    //     }
    // }
}

void ASimulationManager::UpdateAllRobots(float DeltaTime)
{
    for (AActor* Robot : RegisteredRobots)
    {
        if (Robot && Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
        {
            ISimulatable::Execute_SimulationUpdate(Robot, DeltaTime);
        }
    }
}

void ASimulationManager::SetSimulationMode(ESimulationMode NewMode)
{
    if (CurrentSimulationMode != NewMode)
    {
        CurrentSimulationMode = NewMode;
        bWaitingForExternalCommand = false;
        
        UE_LOG(LogTemp, Display, TEXT("Simulation mode changed to: %s"), 
               *UEnum::GetValueAsString(NewMode));
        
        // Reset time controller when switching modes
        if (TimeController)
        {
            TimeController->Reset();
        }
    }
}

void ASimulationManager::SetTimeScale(float NewTimeScale)
{
    if (TimeController)
    {
        TimeController->SetTimeScale(NewTimeScale);
    }
}

void ASimulationManager::ResetSimulation()
{
    CurrentSimulationTime = 0.0f;
    CurrentStep = 0;
    
    if (TimeController)
    {
        TimeController->Reset();
    }
    
    // Reset all robots
    for (AActor* Robot : RegisteredRobots)
    {
        if (Robot && Robot->GetClass()->ImplementsInterface(USimulatable::StaticClass()))
        {
            ISimulatable::Execute_ResetRobot(Robot);
        }
    }
    
    UE_LOG(LogTemp, Display, TEXT("Simulation reset"));
}

void ASimulationManager::PausePhysics()
{
    if (UWorld* World = GetWorld())
    {
        // Use time dilation to effectively pause physics
        World->GetWorldSettings()->SetTimeDilation(0.0001f);
        
        UE_LOG(LogTemp, Display, TEXT("Physics paused via time dilation"));
    }
}
void ASimulationManager::ResumePhysics()
{
    if (UWorld* World = GetWorld())
    {
        // Restore normal time dilation
        World->GetWorldSettings()->SetTimeDilation(1.0f);
        
        UE_LOG(LogTemp, Display, TEXT("Physics resumed"));
    }
}

void ASimulationManager::RequestSimulationStep()
{
    if (CurrentSimulationMode == ESimulationMode::Lockstep && bWaitingForExternalCommand)
    {
        bWaitingForExternalCommand = false;
        UE_LOG(LogTemp, Verbose, TEXT("External step command received"));
    }
}

void ASimulationManager::StartNewEpisode()
{
    CurrentEpisode++;
    CurrentStep = 0;
    ResetSimulation();
    
    UE_LOG(LogTemp, Display, TEXT("Started episode %d"), CurrentEpisode);
}

void ASimulationManager::DrawImGuiWindow()
{
    if (!bShowImGuiWindow)
    {
        return;
    }
    
    ImGui::Begin("Simulation Manager", &bShowImGuiWindow);
    
    // Simulation Info
    ImGui::Text("Simulation Time: %.2f s", CurrentSimulationTime);
    ImGui::Text("Episode: %d, Step: %d", CurrentEpisode, CurrentStep);
    ImGui::Text("Registered Robots: %d", RegisteredRobots.Num());
    
    ImGui::Separator();
    
    // Mode Selection
    ImGui::Text("Simulation Mode:");
    const char* modes[] = { "Realtime", "Lockstep", "Fast Forward", "Paused" };
    int currentMode = (int)CurrentSimulationMode;
    if (ImGui::Combo("Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
    {
        SetSimulationMode((ESimulationMode)currentMode);
    }
    
    // Time Scale (for Fast Forward mode)
    if (CurrentSimulationMode == ESimulationMode::FastForward)
    {
        float timeScale = TimeController ? TimeController->GetTimeScale() : 1.0f;
        if (ImGui::SliderFloat("Time Scale", &timeScale, 0.1f, 10.0f))
        {
            SetTimeScale(timeScale);
        }
    }
    
    // Fixed Timestep
    if (TimeController)
    {
        float fixedTimestep = TimeController->GetFixedDeltaTime();
        if (ImGui::InputFloat("Fixed Timestep", &fixedTimestep, 0.001f, 0.01f, "%.4f"))
        {
            TimeController->SetFixedTimestep(FMath::Clamp(fixedTimestep, 0.001f, 0.1f));
        }
    }
    
    ImGui::Separator();
    
    // Control Buttons
    if (ImGui::Button("Reset Simulation"))
    {
        ResetSimulation();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("New Episode"))
    {
        StartNewEpisode();
    }
    
    // Manual Step (for Lockstep/Paused modes)
    if (CurrentSimulationMode == ESimulationMode::Lockstep || CurrentSimulationMode == ESimulationMode::Paused)
    {
        if (ImGui::Button("Step"))
        {
            RequestSimulationStep();
            if (CurrentSimulationMode == ESimulationMode::Paused)
            {
                StepSimulation(TimeController->GetFixedDeltaTime());
            }
        }
        
        if (CurrentSimulationMode == ESimulationMode::Lockstep)
        {
            ImGui::SameLine();
            ImGui::Text(bWaitingForExternalCommand ? "Waiting for command..." : "Processing...");
        }
    }
    
    ImGui::Separator();
    
    // Robot List
    ImGui::Text("Registered Robots:");
    ImGui::BeginChild("RobotList", ImVec2(0, 150), true);
    for (int32 i = 0; i < RegisteredRobots.Num(); i++)
    {
        if (RegisteredRobots[i])
        {
            bool isSelected = (i == SelectedRobotIndex);
            if (ImGui::Selectable(TCHAR_TO_UTF8(*RegisteredRobots[i]->GetName()), isSelected))
            {
                SelectedRobotIndex = i;
            }
        }
    }
    ImGui::EndChild();
    
    ImGui::End();
}