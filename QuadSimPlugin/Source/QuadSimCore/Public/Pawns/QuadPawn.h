#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Core/ThrusterComponent.h"
#include "Utility/NavigationComponent.h"
#include "UI/ImGuiUtil.h"
#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h" // Add this include
#include "Engine/TextureRenderTarget2D.h" // Ensure this is included
#include "QuadPawn.generated.h"

// Forward Declarations
class UQuadDroneController;
class UImGuiUtil;
class UThrusterComponent;
class UQuadHUDWidget;

// Enum to track camera state
UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	ThirdPerson UMETA(DisplayName = "Third Person"),
	FPV         UMETA(DisplayName = "First Person"),
	GroundTrack UMETA(DisplayName = "Ground Track")
};

enum class EWaypointMode
{
	WaitingForModeSelection,	
	ManualWaypointInput,
	ReadyToStart
};

enum class EFlightMode : uint8;              // lives in the controller

USTRUCT(BlueprintType)
struct FGamepadInputs
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float Throttle = 0.f; // left-stick Y (-1…+1)
	UPROPERTY(BlueprintReadWrite) float Yaw      = 0.f; // left-stick X
	UPROPERTY(BlueprintReadWrite) float Pitch    = 0.f; // right-stick Y
	UPROPERTY(BlueprintReadWrite) float Roll     = 0.f; // right-stick X
};


UCLASS()
class QUADSIMCORE_API AQuadPawn : public APawn 
{
	GENERATED_BODY()

public:
	// Constructor
	AQuadPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// --- Drone Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DroneBody;

	// --- Camera Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraFPV;

	// Ground tracking camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraGroundTrack;
	
	// --- HUD Scene Capture Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	USceneCaptureComponent2D* TPCaptureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	USceneCaptureComponent2D* FPVCaptureComponent;

	// --- HUD Render Targets ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	UTextureRenderTarget2D* TPCaptureRenderTarget;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	UTextureRenderTarget2D* FPVCaptureRenderTarget;

	// --- Thruster Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<UStaticMeshComponent*> Propellers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TArray<UThrusterComponent*> Thrusters;
 	
	// ROS2 Controller as a child actor component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* ROS2ControllerComponent;

	// --- Drone Configuration ---
	UPROPERTY(EditDefaultsOnly, Category = "Drone Configuration")
	TArray<bool> MotorClockwiseDirections = { false, true, true, false }; // FL, FR, BL, BR

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drone State")
	TArray<float> PropellerRPMs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	UQuadDroneController* QuadController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UImGuiUtil* ImGuiUtil;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Identification")
	FString DroneID;

	EWaypointMode WaypointMode;
	TArray<FVector> ManualWaypoints;
	FVector NewWaypoint;

	// --- Helper Functions ---
	void SwitchCamera();
	void ToggleImguiInput();
	void ReloadJSONConfig();

	UFUNCTION(BlueprintPure, Category = "Drone State")
	float GetMass();

	bool getCollisionState(){return bHasCollidedWithObstacle;}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	bool bHasCollidedWithObstacle;

	UFUNCTION(BlueprintPure, Category = "Collision")
	bool HasCollided() const { return bHasCollidedWithObstacle; }

	UFUNCTION(BlueprintCallable, Category = "Collision")
	void ResetCollisionStatus();

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
   UNavigationComponent* NavigationComponent;

   // Generate a figure-8 waypoint list around the pawn's current position
   UFUNCTION(BlueprintCallable, Category = "Navigation")
   TArray<FVector> GenerateFigureEightWaypoints() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input")
	FGamepadInputs GamepadInputs;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY()
	UQuadHUDWidget* HUDWidgetInstance;
	void UpdateHUD();
	

    UFUNCTION()
    void OnDroneHit(
       UPrimitiveComponent* HitComponent,
       AActor* OtherActor,
       UPrimitiveComponent* OtherComp,
       FVector NormalImpulse,
       const FHitResult& Hit);

	void OnThrottleAxis(float Value);
	void OnYawAxis     (float Value);
	void OnPitchAxis   (float Value);
	void OnRollAxis    (float Value);
	void ToggleGamepadMode();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	ECameraMode CurrentCameraMode;
	void ResetGroundCameraPosition();
	void UpdateGroundCameraTracking();



private:
	void UpdateControl(float DeltaTime);
	float LastCollisionTime;
	float CollisionTimeout = 0.2f;
	bool bWaypointModeSelected;

};