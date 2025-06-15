#pragma once

#include "CoreMinimal.h"
#include "Utility/QuadPIDController.h"
#include "UI/ImGuiUtil.h"
#include "QuadDroneController.generated.h"

class AQuadPawn;

UENUM(BlueprintType)
enum class EFlightMode : uint8
{
    None UMETA(DisplayName = "None"),
    AutoWaypoint UMETA(DisplayName = "AutoWaypoint"),
    JoyStickControl UMETA(DisplayName = "JoyStickControl"),
    VelocityControl UMETA(DisplayName = "VelocityControl")
};

USTRUCT()
struct FFullPIDSet
{
    GENERATED_BODY()

    QuadPIDController* XPID;
    QuadPIDController* YPID;
    QuadPIDController* ZPID;
    QuadPIDController* RollPID;
    QuadPIDController* PitchPID;
    QuadPIDController* RollRatePID;
    QuadPIDController* PitchRatePID;
    QuadPIDController* YawRatePID;
    FFullPIDSet()
        : XPID(nullptr)
        , YPID(nullptr)
        , ZPID(nullptr)
        , RollPID(nullptr)
        , PitchPID(nullptr)
        , PitchRatePID(nullptr)
        , RollRatePID(nullptr)
        , YawRatePID(nullptr)
    {
    }
};


UCLASS(Blueprintable, BlueprintType)
class QUADSIMCORE_API UQuadDroneController : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY()
    AQuadPawn* dronePawn;
    UPROPERTY()
    TArray<float> Thrusts;

    UQuadDroneController(const FObjectInitializer& ObjectInitializer);

    void Initialize(AQuadPawn* InPawn);
    void Update(double DeltaTime);

    //void ApplyControllerInput(double a_deltaTime);
    void FlightController(double DeltaTime);
    void dynamicController(double DeltaTime);
    void ThrustMixer(double currentRoll, double currentPitch, double zOutput, double rollOutput, double pitchOutput);
    void YawRateControl(double DeltaTime);
    
    void ResetPID();
    void ResetDroneIntegral();
    void ResetDroneHigh();
    void ResetDroneOrigin();

    void SetDesiredVelocity(const FVector& NewVelocity);
    void SetManualThrustMode(bool bEnable);
    void SetHoverMode(bool bActive, float TargetAltitude = 250.0f);
    void SetDestination(FVector desiredSetPoints);

    void DrawDebugVisuals(const FVector& currentPosition)const;
    void DrawDebugVisualsVel(const FVector& horizontalVelocity) const;
    void SafetyReset();
    void ApplyManualThrusts();
 
    FVector GetDesiredVelocity() const { return desiredNewVelocity; }
    bool GetDebugVisualsEnabled() const { return bDebugVisualsEnabled; }
    FVector GetCurrentLocalVelocity() const { return currentLocalVelocity; }
    float GetDesiredYawRate() const { return desiredYawRate; }
    double GetDesiredRoll() const {return desiredRoll;}
    double GetDesiredPitch() const {return desiredPitch;}
    FVector GetCurrentSetPoint() const { return setPoint; }
    const TArray<float>& GetThrusts() const { return Thrusts; }

    void SetDebugVisualsEnabled(bool bEnabled) { bDebugVisualsEnabled = bEnabled; }
    void SetDesiredYawRate(float NewYawRate) { desiredYawRate = NewYawRate; }
    void SetDesiredAngle(float newAngle) { maxAngle = newAngle; }
    void SetMaxVelocity(float newMaxVelocity) { maxVelocity = newMaxVelocity;}
    void SetMaxAngle(float newMaxAngle) { maxAngle = newMaxAngle;}
    bool IsHoverModeActive() const { return bHoverModeActive; }

    float GetCurrentThrustOutput(int32 ThrusterIndex) const;
    UFUNCTION(BlueprintPure, Category = "Drone State")
    FVector GetCurrentVelocity() const; 

    UFUNCTION(BlueprintCallable, Category = "Flight")
    void SetFlightMode(EFlightMode NewMode);
    FFullPIDSet* GetPIDSet(EFlightMode Mode)
    {
        return &PIDSet;
    }
    
private:

    UPROPERTY()
    FFullPIDSet PIDSet;
    QuadPIDController* AltitudePID;
    EFlightMode currentFlightMode;

    //Global Drone Variables
    FVector currentLocalVelocity;
    float maxVelocity;
    float maxAngle;
    float maxPIDOutput;
    FVector desiredForwardVector;
    double YawTorqueForce;
    double LastYawTorqueApplied; // Maybe not needed, why global??
    float desiredYawRate;
    double desiredRoll;
    double desiredPitch;
    bool bDebugVisualsEnabled = false;

    // AutoWaypointControl variables
    FVector setPoint;
    float minAltitudeLocal;
    float acceptableDistance;
    // Cascaded yaw control parameters
    float maxYawRate;
    float minVelocityForYaw;

    // VelocityControl
    FVector desiredNewVelocity;

    // Hover Mode
    float hoverTargetAltitude;
    bool bHoverModeActive;

    // Debug
    bool Debug_DrawDroneCollisionSphere;
    bool Debug_DrawDroneWaypoint;
    
    bool bManualThrustMode = false;


};