#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimulationManager.generated.h"

class UTimeController;

UENUM(BlueprintType)
enum class ESimMode : uint8
{
    Cinematic     UMETA(DisplayName = "Cinematic"),   // engine time scaled
    Robotics      UMETA(DisplayName = "Robotics"),    // fixed-dt steppable + SimSpeed
    Lockstep      UMETA(DisplayName = "PX4 Lockstep"),
    Paused        UMETA(DisplayName = "Paused")
};

UCLASS(Blueprintable)
class SIMULATIONCORE_API ASimulationManager : public AActor
{
    GENERATED_BODY()

public:
    ASimulationManager();

    virtual void Tick(float DeltaTime) override;

    // Mode
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void SetMode(ESimMode NewMode);
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    ESimMode GetMode() const { return Mode; }

    // Robotics mode speed control (does nothing in Cinematic/Lockstep)
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void SetSimSpeed(float NewSpeed);
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    float GetSimSpeed() const;

    // Cinematic time dilation control (does nothing in Robotics/Lockstep)
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void SetCinematicTimeScale(float NewScale);
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    float GetCinematicTimeScale() const { return CinematicTimeScale; }

    // Stepping controls
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void Pause();
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void Resume();                // resumes to Robotics by default
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void StepOnce();              // single step (Robotics: one dt; Lockstep: one PX4 step)

    // Episode utilities for RL
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void ResetSimulation();
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    int32 GetCurrentEpisode() const { return CurrentEpisode; }
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    int32 GetCurrentStep() const { return CurrentStep; }

    // Robot registry (ISimulatable)
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void RegisterRobot(AActor* Robot);
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void UnregisterRobot(AActor* Robot);
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    TArray<AActor*>& GetRegisteredRobots() { return RegisteredRobots; }

    // Utility
    static ASimulationManager* Get(UWorld* World);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // Per-frame orchestrations by mode
    void Tick_Cinematic(float DeltaTime);
    void Tick_Robotics(float DeltaTime);
    void Tick_Lockstep(float DeltaTime);

    // Executes exactly one fixed simulation step (physics + sensors + robots)
    void ExecuteFixedStep(float FixedDeltaTime);

    // Updates all ISimulatable robots using fixed dt
    void UpdateAllRobots(float FixedDeltaTime);

    // Load-shedding hook to keep FPS >= MinTargetFPS
    void ShedLoadIfNeeded();

private:
    // Time controller (Robotics/Lockstep)
    UPROPERTY()
    UTimeController* TimeController = nullptr;

    // Mode
    UPROPERTY(EditAnywhere, Category = "Simulation")
    ESimMode Mode = ESimMode::Robotics;   // Default: Robotics

    // Cinematic engine time scale
    UPROPERTY(EditAnywhere, Category = "Simulation")
    float CinematicTimeScale = 1.0f;

    // Accounting
    UPROPERTY(VisibleAnywhere, Category = "Simulation")
    double SimTimeSeconds = 0.0;
    UPROPERTY(VisibleAnywhere, Category = "Simulation")
    int32 CurrentEpisode = 0;
    UPROPERTY(VisibleAnywhere, Category = "Simulation")
    int32 CurrentStep = 0;

    // PX4 lockstep flag (when waiting for an external “continue”)
    UPROPERTY(VisibleAnywhere, Category = "Simulation")
    bool bWaitingForExternalCommand = false;

    // Performance guardrails
    UPROPERTY(EditAnywhere, Category = "Performance")
    int32 MaxStepsPerFrame = 10;
    UPROPERTY(EditAnywhere, Category = "Performance")
    float MinTargetFPS = 30.0f;

    // Physics settings backup
    float OriginalMaxPhysicsStep = 0.f;
    bool  OriginalSubstepping = false;

    // Robots
    UPROPERTY(VisibleAnywhere, Category = "Simulation")
    TArray<AActor*> RegisteredRobots;

    // UI state
    bool bStepRequested = false;
};
