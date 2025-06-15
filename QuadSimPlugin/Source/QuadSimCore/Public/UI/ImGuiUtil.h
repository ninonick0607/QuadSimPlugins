// ImGuiUtil.h
#pragma once
#include "CoreMinimal.h"
#include "ImGuiUtil.generated.h"
// Forward declarations
class AQuadPawn;
class QuadPIDController;
class UQuadDroneController;

enum class EFlightMode : uint8;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QUADSIMCORE_API UImGuiUtil : public UActorComponent
{
    GENERATED_BODY()
public:

    UImGuiUtil();

    /** Call this once the owning controller and pawn are valid */
    void Initialize(AQuadPawn* InPawn, UQuadDroneController* InController);

    /** Main functions to draw the UI */
    void ImGuiHud(EFlightMode CurrentMode, float deltaTime);
    
    void RenderControlPlots(float deltaTime, const FRotator& currentRotation, float desiredRoll, float desiredPitch,float maxAngle);
    void DisplayPIDSettings(EFlightMode Mode, const char* headerLabel, bool& synchronizeXYGains, bool& synchronizeGains);
    void DisplayButtons();
    void DisplayThrust(TArray<float>& ThrustsNum);
    void DisplayDesiredVelocities(float maxVelocity);
    void DisplayDesiredPositions();
    void DisplayPIDHistoryWindow();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
 

    UPROPERTY()
    AQuadPawn* DronePawn;

    UPROPERTY()
    UQuadDroneController* Controller;

    float maxVelocityBound;
    bool plotSwitch;
    float maxThrust;
    // Persistent slider values for runtime adjustments
    UPROPERTY()
    float SliderMaxVelocity;
    UPROPERTY()
    float SliderMaxAngle;
    // Data for plotting
    TArray<float> TimeData;
    TArray<float> Thrust0Data;
    TArray<float> Thrust1Data;
    TArray<float> Thrust2Data;
    TArray<float> Thrust3Data;
    float CumulativeTime;
    float MaxPlotTime;
    TArray<float> DesiredYawData;
    TArray<float> CurrentYawData;
    TArray<float> DesiredHeadingData;  
    TArray<float> CurrentHeadingData;  
    TArray<float> VectorErrorData;     

    // New data members for control plots
    TArray<float> CurrentVelocityXData;
    TArray<float> CurrentVelocityYData;
    TArray<float> CurrentVelocityZData;
    TArray<float> DesiredVelocityXData;
    TArray<float> DesiredVelocityYData;
    TArray<float> DesiredVelocityZData;
    TArray<float> CurrentRollData;
    TArray<float> DesiredRollData;
    TArray<float> CurrentPitchData;
    TArray<float> DesiredPitchData;
    
    // Helper method to load PID values from a CSV row
    void LoadPIDValues(EFlightMode Mode, const TArray<FString>& Values);

    int32 MaxDataPoints;
    bool bShowSettingsUI;
};