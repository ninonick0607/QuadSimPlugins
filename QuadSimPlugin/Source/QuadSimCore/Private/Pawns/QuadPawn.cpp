#include "Pawns/QuadPawn.h"
#include "Controllers/QuadDroneController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Core/DroneJSONConfig.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h" 
#include "GameFramework/Actor.h"        
#include "Core/ThrusterComponent.h"       
#include "UI/ImGuiUtil.h"
#include "UI/QuadHUDWidget.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimulationCore/Public/Interfaces/ISimulatable.h" // Add this for interface check

#define EPSILON 0.0001f
// At the top of QuadPawn.cpp

namespace DroneWaypointConfig
{
	static constexpr float startHeight = 500.0f;
	static constexpr float maxHeight = 1000.0f;
	static constexpr float radius = 1000.0f;
	static constexpr float heightStep = 100.0f;
	static constexpr int32 pointsPerLoop = 8;
	static constexpr float angleStep = 2.0f * PI / pointsPerLoop;
}

const FVector start = FVector(0, 0, 1000);

// Generate a spiral of waypoints around a given start position
static TArray<FVector> spiralWaypoints(const FVector& startPos)
{
    TArray<FVector> waypoints;
    // Initial ascent to specified start height
    FVector basePos = startPos;
    const float initialAltitude = DroneWaypointConfig::startHeight;
    const float targetZ = basePos.Z + initialAltitude;
    waypoints.Add(FVector(basePos.X, basePos.Y, targetZ));

    // Figure-8 parameters
    // Increase resolution for smoother curves
    const int32 numPoints = 360;
    const float twoPi = 2.0f * PI;
    // Dimensions: roughly a 10m x 10m box (cm units)
    const float width = DroneWaypointConfig::radius * 0.5f; // ~5m half-width
    const float height = width;                            // symmetrical loops

    // Generate waypoints along the figure-8 curve at constant altitude
    for (int32 i = 0; i <= numPoints; ++i)
    {
        float t = twoPi * static_cast<float>(i) / static_cast<float>(numPoints);
        float x = basePos.X + width * FMath::Sin(t);
        float y = basePos.Y + height * FMath::Sin(2.0f * t);
        waypoints.Add(FVector(x, y, targetZ));
    }
    return waypoints;
}

// Expose figure-8 waypoint generator for the pawn

const FName ObstacleCollisionTag = FName("Obstacle");
TArray<FVector> AQuadPawn::GenerateFigureEightWaypoints() const
{
    return spiralWaypoints(GetActorLocation());
}

AQuadPawn::AQuadPawn()
	: DroneBody(nullptr)
	, SpringArm(nullptr)
	, Camera(nullptr)
	, CameraFPV(nullptr)
	, CameraGroundTrack(nullptr)
	, QuadController(nullptr)	
	, ImGuiUtil(nullptr)
	, WaypointMode(EWaypointMode::WaitingForModeSelection)
	, NewWaypoint(FVector::ZeroVector)
	, bHasCollidedWithObstacle(false)
	, CurrentCameraMode(ECameraMode::ThirdPerson)
	, bWaypointModeSelected(false)
{
	PrimaryActorTick.bCanEverTick = true;

    // Skeletal mesh for drone body (physics & visuals)
    DroneBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneBody"));
    RootComponent = DroneBody;
    DroneBody->SetSimulatePhysics(true);
    DroneBody->SetNotifyRigidBodyCollision(true);
    DroneBody->SetGenerateOverlapEvents(true);
    DroneBody->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	
	CameraFPV = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFPV"));
	CameraFPV->SetupAttachment(DroneBody,TEXT("FPVCam"));
	CameraFPV->bAutoActivate = true; 

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(DroneBody);
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); 
	Camera->bAutoActivate = false; 
	
	CameraGroundTrack = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraGroundTrack"));
	CameraGroundTrack->bAutoActivate = false;


	// Third-Person Capture (attaches to the spring arm like the main TP camera)
	TPCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("TPCaptureComponent"));
	TPCaptureComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	TPCaptureComponent->bCaptureEveryFrame = false; // IMPORTANT! For performance. We'll trigger captures manually.

	// FPV Capture (attaches to the body like the main FPV camera)
	FPVCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FPVCaptureComponent"));
	FPVCaptureComponent->SetupAttachment(DroneBody, TEXT("FPVCam"));
	FPVCaptureComponent->bCaptureEveryFrame = false; // IMPORTANT!

	
	const FString propellerNames[] = { TEXT("MotorFL"), TEXT("MotorFR"), TEXT("MotorBL"), TEXT("MotorBR") };
	const FString socketNames[] = { TEXT("MotorSocketFL"), TEXT("MotorSocketFR"), TEXT("MotorSocketBL"), TEXT("MotorSocketBR") };

	Propellers.SetNum(4);
	Thrusters.SetNum(4);
	PropellerRPMs.SetNum(4);

	for (int i = 0; i < 4; i++)
	{
		Propellers[i] = CreateDefaultSubobject<UStaticMeshComponent>(*propellerNames[i]);
		Propellers[i]->SetSimulatePhysics(false);
		Propellers[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Propellers[i]->SetupAttachment(DroneBody, *socketNames[i]);

		Thrusters[i] = CreateDefaultSubobject<UThrusterComponent>(
			*FString::Printf(TEXT("Thruster_%s"), *propellerNames[i])
		);
		Thrusters[i]->SetupAttachment(DroneBody, *socketNames[i]);
		Thrusters[i]->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

		PropellerRPMs[i] = 0.f;

	}

	// Create additional components
	ImGuiUtil = CreateDefaultSubobject<UImGuiUtil>(TEXT("DroneImGuiUtil"));
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	NavigationComponent = CreateDefaultSubobject<UNavigationComponent>(TEXT("NavigationComponent"));

}

void AQuadPawn::BeginPlay()
{
	Super::BeginPlay();


	// Assign the render target assets to the capture components
	if (TPCaptureRenderTarget)
	{
		TPCaptureComponent->TextureTarget = TPCaptureRenderTarget;
	}
	if (FPVCaptureRenderTarget)
	{
		FPVCaptureComponent->TextureTarget = FPVCaptureRenderTarget;
	}

	// --- Create and add HUD to viewport ---
	// You might want to create a C++ property for the HUD class to use
	// For now, we assume you have a Blueprint of WBP_DroneHUD
	if (IsLocallyControlled() && HUDWidgetClass) // Check if player controlled AND if a widget class was set in the editor
	{
		APlayerController* PC = GetController<APlayerController>();
		if (PC)
		{
			// Use the correct CreateWidget function
			// This uses the HUDWidgetClass property you set in the editor.
			HUDWidgetInstance = CreateWidget<UQuadHUDWidget>(PC, HUDWidgetClass);
			if (HUDWidgetInstance)
			{
				HUDWidgetInstance->AddToViewport();
			}
		}
	}
	
	DroneID = GetName();
	UE_LOG(LogTemp, Display, TEXT("QuadPawn BeginPlay: DroneID set to %s"), *DroneID);
	
	if (!QuadController)
	{
		QuadController = NewObject<UQuadDroneController>(this, TEXT("QuadDroneController"));
		QuadController->Initialize(this);
	}
	UE_LOG(LogTemp, Warning, TEXT("Debug SetController: QuadController=%p"), QuadController);
	
	
	UE_LOG(LogTemp, Display, TEXT("QuadPawn BeginPlay: Pawn=%p, Name=%s"), this, *GetName());
	
	if (!ImGuiUtil)
	{
		ImGuiUtil = NewObject<UImGuiUtil>(this, UImGuiUtil::StaticClass(), TEXT("DroneImGuiUtil"));
		ImGuiUtil->Initialize(this, QuadController);
	}
    if (ImGuiUtil)
    {
        ImGuiUtil->Initialize(this, QuadController);
    }

	
    // Reset PID controllers
    QuadController->ResetPID();
    // Collision events binding
    if (DroneBody)
    {
        DroneBody->OnComponentHit.AddDynamic(this, &AQuadPawn::OnDroneHit);
    }
    // Collision handling via NotifyHit override; skip AddDynamic binding
	
	ResetCollisionStatus();

	Camera->SetActive(false);
	CameraFPV->SetActive(true);
	CameraGroundTrack->SetActive(false);
	CurrentCameraMode = ECameraMode::FPV;

	UpdateHUD();

}

void AQuadPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);
}

void AQuadPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if SimulationManager is controlling updates
	bool bSimulationControlled = false;
	if (UWorld* World = GetWorld())
	{
		// If time dilation is very low, we're being controlled by SimulationManager
		float TimeDilation = World->GetWorldSettings()->TimeDilation;
		if (TimeDilation < 0.01f)
		{
			bSimulationControlled = true;
		}
	}

	// Only update controller if NOT controlled by SimulationManager
	if (!bSimulationControlled)
	{
		UpdateControl(DeltaTime);
	}

	// Visual updates (propellers) should always happen
	for (int32 i = 0; i < Propellers.Num(); i++)
	{
		if (Propellers[i])
		{
			float CurrentThrustVal = QuadController->GetCurrentThrustOutput(i);
			PropellerRPMs[i] = FMath::Abs(CurrentThrustVal) * 1;
			float DirectionMultiplier = 1.0f;
			if (MotorClockwiseDirections.IsValidIndex(i))
			{
				DirectionMultiplier = MotorClockwiseDirections[i] ? -1.0f : 1.0f;
			}
			float DegreesPerSecond = PropellerRPMs[i] * 6.0f;
			float DeltaRotation = DegreesPerSecond * DeltaTime * DirectionMultiplier;
			Propellers[i]->AddLocalRotation(FRotator(0.f, DeltaRotation, 0.f));
		}
	}

	UpdateGroundCameraTracking();

	if (bHasCollidedWithObstacle)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastCollisionTime > CollisionTimeout)
		{
			bHasCollidedWithObstacle = false;
			UE_LOG(LogTemp, Display, TEXT("%s collision status cleared due to inactivity."), *GetName());
		}
	}
}

void AQuadPawn::UpdateControl(float DeltaTime)
{
	if (QuadController)
	{	
		QuadController->Update(DeltaTime);
	}

	if (NavigationComponent)
	{
		NavigationComponent->UpdateNavigation(GetActorLocation());
		FVector NextGoal = NavigationComponent->GetCurrentSetpoint();
		if (QuadController)
		{
			QuadController->SetDestination(NextGoal);
		}
	}
}

void AQuadPawn::SwitchCamera()
{
	if (!Camera || !CameraFPV || !CameraGroundTrack)
	{
		UE_LOG(LogTemp, Warning, TEXT("SwitchCamera: One or more camera components are missing!"));
		return;
	}

	Camera->SetActive(false);
	CameraFPV->SetActive(false);
	CameraGroundTrack->SetActive(false);

	switch (CurrentCameraMode)
	{
	case ECameraMode::ThirdPerson:
		CurrentCameraMode = ECameraMode::FPV;
		CameraFPV->SetActive(true);
		UE_LOG(LogTemp, Log, TEXT("Camera Mode: FPV"));
		break;

	case ECameraMode::FPV:
		CurrentCameraMode = ECameraMode::GroundTrack;
		CameraGroundTrack->SetActive(true);
		ResetGroundCameraPosition();
		UE_LOG(LogTemp, Log, TEXT("Camera Mode: Ground Track"));
		break;
	default:
		CurrentCameraMode = ECameraMode::ThirdPerson;
		Camera->SetActive(true);
		break;

	}
	UpdateHUD();

}


void AQuadPawn::ResetGroundCameraPosition()
{
	if (!CameraGroundTrack || !DroneBody) return;

	const float GroundOffsetDistance = 200.0f; // 5 meters

	FVector DroneLocation = GetActorLocation();
	FRotator DroneYawRotation(0, GetActorRotation().Yaw, 0);

	FVector RightVector = UKismetMathLibrary::GetRightVector(DroneYawRotation);

	FVector GroundPos = FVector(DroneLocation.X, DroneLocation.Y, 10.0f);
	FVector CameraTargetPosition = GroundPos + RightVector * GroundOffsetDistance;

	CameraGroundTrack->SetWorldLocation(CameraTargetPosition);

	UpdateGroundCameraTracking();
}

void AQuadPawn::UpdateGroundCameraTracking()
{
	if (CurrentCameraMode == ECameraMode::GroundTrack && CameraGroundTrack && CameraGroundTrack->IsActive() && DroneBody)
	{
		FVector CameraLocation = CameraGroundTrack->GetComponentLocation();
		FVector DroneLocation = GetActorLocation(); 

		if (FVector::DistSquaredXY(CameraLocation, DroneLocation) < 1.0f) 
		{
			return;
		}

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, DroneLocation);

		FRotator CurrentRotation = CameraGroundTrack->GetComponentRotation();
		FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, GetWorld()->GetDeltaSeconds(), 10.0f); // Adjust interp speed
		CameraGroundTrack->SetWorldRotation(TargetRotation);

	}
}


void AQuadPawn::ToggleImguiInput()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("ImGui.ToggleInput");
}

void AQuadPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("ToggleImGui", IE_Pressed, this, &AQuadPawn::ToggleImguiInput);
	PlayerInputComponent->BindAction("ReloadJSON", IE_Pressed, this, &AQuadPawn::ReloadJSONConfig);

	PlayerInputComponent->BindAxis("GP_Throttle", this, &AQuadPawn::OnThrottleAxis);
	PlayerInputComponent->BindAxis("GP_Yaw",      this, &AQuadPawn::OnYawAxis);
	PlayerInputComponent->BindAxis("GP_Pitch",    this, &AQuadPawn::OnPitchAxis);
	PlayerInputComponent->BindAxis("GP_Roll",     this, &AQuadPawn::OnRollAxis);

	PlayerInputComponent->BindAction("GP_ToggleFlightMode", IE_Pressed,
							this, &AQuadPawn::ToggleGamepadMode);
}

void AQuadPawn::OnThrottleAxis(float Value) { GamepadInputs.Throttle = Value; }
void AQuadPawn::OnYawAxis(float Value)      { GamepadInputs.Yaw      = Value; }
void AQuadPawn::OnPitchAxis(float Value)    { GamepadInputs.Pitch    = Value; }
void AQuadPawn::OnRollAxis(float Value)     { GamepadInputs.Roll     = Value; }

void AQuadPawn::ToggleGamepadMode()
{
	const bool bGP = (QuadController->GetFlightMode() == EFlightMode::JoyStickAngleControl);
	QuadController->SetFlightMode(bGP ? EFlightMode::None
									  : EFlightMode::JoyStickAngleControl);
}

void AQuadPawn::ReloadJSONConfig()
{
	UDroneJSONConfig::Get().ReloadConfig();
}

// Component hit callback
void AQuadPawn::OnDroneHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(ObstacleCollisionTag))
    {
        LastCollisionTime = GetWorld()->GetTimeSeconds();
        if (!bHasCollidedWithObstacle)
        {
            bHasCollidedWithObstacle = true;
            UE_LOG(LogTemp, Display, TEXT("%s collided with obstacle: %s"), *GetName(), *OtherActor->GetName());
        }
    }
}

void AQuadPawn::ResetCollisionStatus()
{
	if (bHasCollidedWithObstacle) 
	{
		UE_LOG(LogTemp, Log, TEXT("%s collision status reset."), *GetName());
	}
	bHasCollidedWithObstacle = false;
}

float AQuadPawn::GetMass()
{
	return DroneBody ? DroneBody->GetMass() : 0.0f;
}

void AQuadPawn::UpdateHUD()
{
	USceneCaptureComponent2D* captureComponentToUse = nullptr;
	UTextureRenderTarget2D* renderTargetToDisplay = nullptr;

	if (CurrentCameraMode == ECameraMode::FPV)
	{
		captureComponentToUse = TPCaptureComponent;
		renderTargetToDisplay = TPCaptureRenderTarget;
	}
	else if (CurrentCameraMode == ECameraMode::ThirdPerson)
	{
		captureComponentToUse = FPVCaptureComponent;
		renderTargetToDisplay = FPVCaptureRenderTarget;
	}
	// Optional: Handle GroundTrack mode here too
	else if (CurrentCameraMode == ECameraMode::GroundTrack)
	{
		captureComponentToUse = FPVCaptureComponent;
		renderTargetToDisplay = FPVCaptureRenderTarget;
	}
    
	if (captureComponentToUse && renderTargetToDisplay)
	{
		captureComponentToUse->CaptureScene();

		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->UpdateHUDTexture(renderTargetToDisplay);
		}
	}
}



