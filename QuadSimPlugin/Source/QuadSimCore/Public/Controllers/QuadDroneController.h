#pragma once

#include "CoreMinimal.h"
#include "Utility/QuadPIDController.h"
#include "ROSFlightControllerSource.h"
#include "UI/ImGuiUtil.h"
#include "QuadDroneController.generated.h"

class AQuadPawn;

UENUM(BlueprintType)
enum class EFlightMode : uint8
{
    None UMETA(DisplayName = "None"),
    AutoWaypoint UMETA(DisplayName = "AutoWaypoint"),
    VelocityControl UMETA(DisplayName = "VelocityControl"),
    AngleControl UMETA(DisplayName = "AngleControl"),
    RateControl UMETA(DisplayName = "RateControl"),
    JoyStickAngleControl UMETA(DisplayName = "JoyStickAngleControl"),
    JoyStickAcroControl UMETA(DisplayName = "JoyStickAcroControl")
};

struct FGamepadInputs;

USTRUCT()
struct FFullPIDSet
{
    GENERATED_BODY()

    QuadPIDController* XPID;
    QuadPIDController* YPID;
    QuadPIDController* ZPID;
    QuadPIDController* RollPID;
    QuadPIDController* PitchPID;
    QuadPIDController* PitchRatePID;
    QuadPIDController* RollRatePID;
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
class QUADSIMCORE_API UQuadDroneController : public UObject, public IROSFlightControllerSource
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

    void FlightController(double DeltaTime);
    void GamepadController(double DeltaTime);
    void dynamicController(double DeltaTime);
    void ThrustMixer(double currentRoll, double currentPitch, double zOutput, double rollOutput, double pitchOutput, double yawOutput);
    float YawRateControl(double DeltaTime);
    
    void ResetPID();
    void ResetDroneIntegral();
    void ResetDroneRotation();
    void ResetDroneOrigin();


    void SetManualThrustMode(bool bEnable);
    void SetHoverMode(bool bActive, float TargetAltitude = 250.0f);
    void SetDestination(FVector desiredSetPoints);

    void DrawDebugVisuals(const FVector& currentPosition)const;
    void DrawDebugVisualsVel(const FVector& horizontalVelocity) const;
    void SafetyReset();
    void ApplyManualThrusts();
 
    FVector GetCurrentLocalVelocity() const { return currentLocalVelocity; }
    FVector GetCurrentAngularVelocity() const {return currentLocalVelocity; }
    FVector GetDesiredVelocity() const { return desiredNewVelocity; }
    bool GetDebugVisualsEnabled() const { return bDebugVisualsEnabled; }
    
    virtual float IsUsingROSflight() const override { return bUseROSflight; }
    virtual float GetDesiredRoll() const override { return desiredRoll; }
    virtual float GetDesiredPitch() const override { return desiredPitch; }
    virtual float GetDesiredYawRate() const override { return desiredYawRate; }
    virtual float GetDesiredThrustNormalized() const override { return desiredThrust_Normalized; }

    
    double GetDesiredRollRate() const {return desiredRollRate;}
    double GetDesiredPitchRate() const {return desiredPitchRate;}
    

    EFlightMode GetFlightMode() const { return currentFlightMode; }
    
    FVector GetCurrentSetPoint() const { return setPoint; }
    const TArray<float>& GetThrusts() const { return Thrusts; }
    
    void SetDebugVisualsEnabled(bool bEnabled) { bDebugVisualsEnabled = bEnabled; }
    void SetDesiredAngle(float newAngle) { maxAngle = newAngle; }
    void SetMaxVelocity(float newMaxVelocity) { maxVelocity = newMaxVelocity;}
    void SetMaxAngle(float newMaxAngle) { maxAngle = newMaxAngle;}
    bool IsHoverModeActive() const { return bHoverModeActive; }

    void SetDesiredVelocity(const FVector& NewVelocity){desiredNewVelocity = NewVelocity;};
    
    void SetDesiredPitchAngle(const float& NewPitch){desiredNewPitch=NewPitch;};
    void SetDesiredRollAngle(const float& NewRoll){desiredNewRoll=NewRoll;};

    void SetDesiredPitchRate(const float& NewPitchRate){desiredNewPitchRate = NewPitchRate;};
    void SetDesiredRollRate(const float& NewRollRate){desiredNewRollRate = NewRollRate;};
    void SetDesiredYawRate(float NewYawRate) { desiredYawRate = NewYawRate; }

    float GetCurrentThrustOutput(int32 ThrusterIndex) const;

    UFUNCTION(BlueprintPure, Category = "Drone State")
    FVector GetCurrentVelocity() const; 
    UFUNCTION(BlueprintPure, Category = "Drone State|ROS")
    FQuat GetOrientationAsQuat() const;
    UFUNCTION(BlueprintPure, Category = "Drone State|ROS")
    FVector GetCurrentAngularVelocityRADPS() const;

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
    bool bDebugVisualsEnabled = false;

    // Position Control
    FVector setPoint;
    
    // Velocity Control
    FVector desiredNewVelocity;

    // Angle Controlv
    float desiredRoll;
    float desiredPitch;
    double desiredNewRoll;
    double desiredNewPitch;

    // Angle Rate Control
    double desiredRollRate;
    double desiredPitchRate;
    double desiredNewRollRate;
    double desiredNewPitchRate;
    float desiredYawRate;
    FVector localAngularRateDeg;
    
    // Hover Mode
    float hoverTargetAltitude;
    bool bHoverModeActive;
    bool bManualThrustMode = false;
    
    bool bUseROSflight = false;

    float desiredThrust_Normalized = 0.0f;

    float maxVelocity;
    float maxAngle;
    float maxAngleRate;
    float maxPIDOutput;
    float minAltitudeLocal;
    float acceptableDistance;

    // Cascaded yaw control parameters
    float maxYawRate;
    float minVelocityForYaw;

    bool Debug_DrawDroneCollisionSphere;
    bool Debug_DrawDroneWaypoint;
    bool bGamepadModeUI = false; 
};
