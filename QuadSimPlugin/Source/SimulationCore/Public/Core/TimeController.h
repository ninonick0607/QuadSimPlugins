#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimeController.generated.h"

UCLASS()
class SIMULATIONCORE_API UTimeController : public UObject
{
    GENERATED_BODY()

public:
    UTimeController();

    // Called each render frame with wall-clock DeltaTime
    void AccumulateTime(float DeltaWallSeconds);

    // Whether at least one fixed step can run
    bool ShouldStep() const;

    // Consume one fixed step from the accumulator
    void ConsumeOneStep();

    // Reset accumulated time (does not change SimTime elsewhere)
    void Reset();

    // --- Getters
    UFUNCTION(BlueprintCallable, Category = "Time Control")
    float GetFixedDeltaTime() const { return FixedTimestep; }

    UFUNCTION(BlueprintCallable, Category = "Time Control")
    float GetAccumulator() const { return Accumulator; }

    UFUNCTION(BlueprintCallable, Category = "Time Control")
    float GetSimSpeed() const { return SimSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Time Control")
    bool IsPaused() const { return bPaused; }

    // --- Setters
    UFUNCTION(BlueprintCallable, Category = "Time Control")
    void SetFixedTimestep(float NewTimestep);

    // SimSpeed multiplies wall time before accumulation (Robotics mode)
    UFUNCTION(BlueprintCallable, Category = "Time Control")
    void SetSimSpeed(float NewSimSpeed);

    UFUNCTION(BlueprintCallable, Category = "Time Control")
    void SetPaused(bool bNewPaused);

protected:
    // Fixed physics dt in seconds (e.g., 0.004 = 250 Hz)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Control")
    float FixedTimestep;

    // Accumulated (scaled) wall time waiting to be stepped
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time Control")
    float Accumulator;

    // Wall->sim scaling used ONLY in Robotics (steppable) mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Control")
    float SimSpeed;

    // Pause flag (prevents accumulation)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time Control")
    bool bPaused;

    // Prevent spiral-of-death
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Control")
    float MaxAccumulatedTime;
};
