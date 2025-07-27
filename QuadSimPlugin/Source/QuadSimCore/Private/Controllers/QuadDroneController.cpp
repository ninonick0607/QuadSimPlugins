// QuadDroneController.cpp

#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "DrawDebugHelpers.h"
#include "imgui.h"
#include <random>

#ifndef EXCLUDE_PX4_COMPONENT
#include "Controllers/PX4Component.h"
#endif
#include "UI/ImGuiUtil.h"
#include "Core/DroneJSONConfig.h"
#include "Core/DroneManager.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Sensors/GPSSensor.h"
#include "Sensors/SensorManagerComponent.h"


// ---------------------- Constructor ------------------------


UQuadDroneController::UQuadDroneController(const FObjectInitializer& ObjectInitializer)
	: dronePawn(nullptr)
	, Thrusts({ 0, 0, 0, 0 })
	, AltitudePID(nullptr)
	, currentLocalVelocity(FVector::ZeroVector)
	, bDebugVisualsEnabled(false)
	, setPoint(FVector::ZeroVector)
	, desiredNewVelocity(FVector::ZeroVector)
	, desiredRoll(0.0f)
	, desiredPitch(0.0f)
	, desiredNewRoll(0.0f)
	, desiredNewPitch(0.0f)
	, desiredRollRate(0.0f)
	, desiredPitchRate(0.0f)
	, desiredNewRollRate(0.0f)
	, desiredNewPitchRate(0.0f)
	, desiredYawRate(0.0f)
	, localAngularRateDeg(FVector::ZeroVector)
	, hoverTargetAltitude(0.0f)
	, bHoverModeActive(false)
	, bManualThrustMode(false)
	
{
	const auto& Config = UDroneJSONConfig::Get().Config;
	maxVelocity = Config.FlightParams.MaxVelocity;
	maxAngle = Config.FlightParams.MaxAngle;
	maxAngleRate = Config.FlightParams.MaxAngleRate;
	maxPIDOutput = Config.FlightParams.MaxPIDOutput;
	minAltitudeLocal = Config.FlightParams.MinAltitudeLocal;
    acceptableDistance = Config.FlightParams.AcceptableDistance;
	
    // Yaw control parameters from config
    maxYawRate = Config.ControllerParams.YawRate;
    minVelocityForYaw = Config.ControllerParams.MinVelocityForYaw;

    // Start with flight mode None (motors off) until mode is selected via UI
    currentFlightMode = EFlightMode::None;
    Debug_DrawDroneCollisionSphere = true;
	Debug_DrawDroneWaypoint = true;
	
	FFullPIDSet ControllerSet;

	ControllerSet.XPID = new QuadPIDController();
	ControllerSet.XPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.XPID->SetGains(0.03f, 0.f, 0.0f);

	ControllerSet.YPID = new QuadPIDController();
	ControllerSet.YPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.YPID->SetGains(0.03f, 0.0f, 0.0f);

	ControllerSet.ZPID = new QuadPIDController();
	ControllerSet.ZPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.ZPID->SetGains(5.f, 0.0f, 0.0f);

	ControllerSet.RollPID = new QuadPIDController();
	ControllerSet.RollPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.RollPID->SetGains(0.31f, 0.2f, 0.34f);

	ControllerSet.PitchPID = new QuadPIDController();
	ControllerSet.PitchPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.PitchPID->SetGains(0.35f, 0.16f, 0.25f);

	ControllerSet.RollRatePID = new QuadPIDController();
	ControllerSet.RollRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.RollRatePID->SetGains(0.35f, 0.16f, 0.25f);
	
	ControllerSet.PitchRatePID = new QuadPIDController();
	ControllerSet.PitchRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.PitchRatePID->SetGains(0.35f, 0.16f, 0.25f);
	
	ControllerSet.YawRatePID = new QuadPIDController();
	ControllerSet.YawRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.YawRatePID->SetGains(1.0f, 0.0f, 0.0f);
//--------------------------------------------------------------
	PIDSet = MoveTemp(ControllerSet);

	AltitudePID = new QuadPIDController();
	AltitudePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AltitudePID->SetGains(5.f, 1.f, 0.1f);

}

// ---------------------- Initialization ------------------------

void UQuadDroneController::Initialize(AQuadPawn* InPawn)
{
	if (!InPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Initialize called with null pawn"));
		return;
	}

	if (dronePawn != InPawn)
	{
		UE_LOG(LogTemp, Display, TEXT("Initializing controller for pawn: %s"), *InPawn->GetName());
		dronePawn = InPawn;
		// Register this controller with the global DroneManager for swarm broadcasts
		if (UWorld* World = dronePawn->GetWorld())
		{
			if (ADroneManager* Manager = ADroneManager::Get(World))
			{
				Manager->RegisterDroneController(this);
			}
		}
	}
}

// ---------------------- Update ------------------------

void UQuadDroneController::Update(double a_deltaTime)
{
    ADroneManager* Manager = ADroneManager::Get(dronePawn ? dronePawn->GetWorld() : nullptr);
    bool bShowUI = true;
    if (Manager)
    {
       if (!Manager->IsSwarmMode())
       {
          APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
          if (PC && PC->GetPawn() != dronePawn)
          {
             bShowUI = false;
          }
       }
       else
       {
          int32 MyIndex = Manager->GetDroneIndex(dronePawn);
          if (MyIndex != 0)
          {
             bShowUI = false;
          }
       }
    }
    if (bShowUI)
    {
       // Unique window name per drone to avoid ID conflicts
       FString WinName = FString::Printf(TEXT("Flight Mode Selector##%s"), *dronePawn->DroneID);
       ImGui::Begin(TCHAR_TO_UTF8(*WinName));

    	static bool useGamepad = false;                         
    	ImGui::Checkbox("GamePad?", &useGamepad);
       
    	// PX4 Control checkbox
    	bool currentPX4State = bUseExternalController;
    	if (ImGui::Checkbox("Use PX4 Control", &currentPX4State))
    	{
    		SetUseExternalController(currentPX4State);
           
    		// If we have a PX4Component, activate/deactivate it
    		if (dronePawn)
    		{
    			if (auto* PX4Comp = dronePawn->FindComponentByClass<UPX4Component>())
    			{
    				PX4Comp->SetPX4Active(currentPX4State);
    			}
    		}
    	}
       
    	ImGui::Separator();

       bGamepadModeUI = useGamepad;               
       if (!useGamepad)
       {
          // determine which button is active
          const bool selPos = currentFlightMode == EFlightMode::AutoWaypoint;
          const bool selVel = currentFlightMode == EFlightMode::VelocityControl;
          const bool selAng = currentFlightMode == EFlightMode::AngleControl;
          const bool selRate = currentFlightMode == EFlightMode::RateControl; // Added check for Rate Control mode

          // Position Control
          ImGui::PushStyleColor(
             ImGuiCol_Button,
             selPos
                ? ImVec4(0.1f, 0.7f, 0.1f, 1.0f)
                : ImGui::GetStyleColorVec4(ImGuiCol_Button)
          );
          if (ImGui::Button("Position Control", ImVec2(200, 50)))
             SetFlightMode(EFlightMode::AutoWaypoint);
          ImGui::PopStyleColor();

          // Move By Velocity
          ImGui::PushStyleColor(
             ImGuiCol_Button,
             selVel
                ? ImVec4(0.1f, 0.7f, 0.1f, 1.0f)
                : ImVec4(ImGui::GetStyleColorVec4(ImGuiCol_Button))
          );
          if (ImGui::Button("Move By Velocity", ImVec2(200, 50)))
             SetFlightMode(EFlightMode::VelocityControl);
          ImGui::PopStyleColor();

          // Angle Control
          ImGui::PushStyleColor(
             ImGuiCol_Button,
             selAng
                ? ImVec4(0.1f, 0.7f, 0.1f, 1.0f)
                : ImGui::GetStyleColorVec4(ImGuiCol_Button)
          );
          if (ImGui::Button("Angle Control", ImVec2(200, 50)))
             SetFlightMode(EFlightMode::AngleControl);
          ImGui::PopStyleColor();
          
          // --- BEGIN ADDED CODE ---
          // Rate Control
          ImGui::PushStyleColor(
             ImGuiCol_Button,
             selRate
                ? ImVec4(0.1f, 0.7f, 0.1f, 1.0f)
                : ImGui::GetStyleColorVec4(ImGuiCol_Button)
          );
          if (ImGui::Button("Rate Control", ImVec2(200, 50)))
             SetFlightMode(EFlightMode::RateControl);
          ImGui::PopStyleColor();
          // --- END ADDED CODE ---
       }
       else
       {
          // highlight the active one with a different colour
          const bool selAngle = currentFlightMode == EFlightMode::JoyStickAngleControl;
          const bool selAcro  = currentFlightMode == EFlightMode::JoyStickAcroControl;

          ImGui::PushStyleColor(ImGuiCol_Button, selAngle ? ImVec4(0.1f,0.7f,0.1f,1.f)
                                              : ImGui::GetStyleColorVec4(ImGuiCol_Button));
          if (ImGui::Button("Angle Control", ImVec2(200,50)))
             SetFlightMode(EFlightMode::JoyStickAngleControl);
          ImGui::PopStyleColor();

          ImGui::PushStyleColor(ImGuiCol_Button, selAcro ? ImVec4(0.1f,0.7f,0.1f,1.f)
                                              : ImGui::GetStyleColorVec4(ImGuiCol_Button));
          if (ImGui::Button("Acro Control", ImVec2(200,50)))
             SetFlightMode(EFlightMode::JoyStickAcroControl);
          ImGui::PopStyleColor();
       }
       ImGui::End();
    }

    if (currentFlightMode != EFlightMode::None)
    {
       if (bGamepadModeUI)
          GamepadController(a_deltaTime);      
       else
          FlightController(a_deltaTime);
    }
}
void UQuadDroneController::GamepadController(double DeltaTime)
{
	if (!dronePawn) return;
	const FGamepadInputs& GP = dronePawn->GamepadInputs;

	/* ---------------- state ---------------- */
	const FVector  currPos = dronePawn->GetActorLocation();
	const FVector  currVel = dronePawn->GetVelocity();
	const FRotator currRot = dronePawn->GetActorRotation();
	const FRotator yawOnlyRot(0.f, currRot.Yaw, 0.f);
	FVector localVel = yawOnlyRot.UnrotateVector(currVel);

	const FVector worldAngDeg = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	localAngularRateDeg = yawOnlyRot.UnrotateVector(worldAngDeg);

	/* ---------------- altitude / throttle ---------------- */
	hoverTargetAltitude += GP.Throttle * 150.f * DeltaTime;          // ±1 m s-¹

	double altVelSetpoint = AltitudePID->Calculate(hoverTargetAltitude, currPos.Z, DeltaTime);
	double zEffort = PIDSet.ZPID->Calculate(altVelSetpoint, localVel.Z, DeltaTime);

	/* ---------------- desired attitudes from sticks ---------------- */
	desiredRoll       =  GP.Roll   *  maxAngle;     // right-stick X
	desiredPitch      = -GP.Pitch  *  maxAngle;     // right-stick Y (invert)
	desiredYawRate    =  GP.Yaw    *  maxYawRate;   // left-stick X

	/* ---------------- PID cascades (same gains as normal) ---------- */
	const double rollOut  = PIDSet.RollPID ->Calculate(desiredRoll,  currRot.Roll,      DeltaTime);
	const double pitchOut = PIDSet.PitchPID->Calculate(desiredPitch, -currRot.Pitch,    DeltaTime);
	const float  yawOut   = YawRateControl(DeltaTime);   // uses desiredYawRate

	/* no XY position hold in this simple ANGLE mode */
	ThrustMixer(desiredRoll, desiredPitch, zEffort,
				rollOut,   pitchOut,      yawOut);

	/* optional HUD / debug */
	DrawDebugVisualsVel(FVector::ZeroVector);

	if (dronePawn && dronePawn->ImGuiUtil)
	{
		// Display HUD only for the selected drone (independent) or all (swarm)
		bool bShowUI = true;
		if (ADroneManager* Manager = ADroneManager::Get(dronePawn->GetWorld()))
		{
			if (!Manager->IsSwarmMode())
			{
				const int32 myIdx = Manager->GetDroneIndex(dronePawn);
				bShowUI = (myIdx == Manager->SelectedDroneIndex);
			}
		}

		if (bShowUI){dronePawn->ImGuiUtil->ImGuiHud(currentFlightMode,DeltaTime);}
	}
}
void UQuadDroneController::FlightController(double DeltaTime)
{
	FFullPIDSet* CurrentSet = &PIDSet;
	if (!CurrentSet || !dronePawn) return;
	
	/* ───── World-space state ───── */
	const FVector  currPos = dronePawn->SensorManager->GPS->GetLastGPS();     
	const FVector  currVel = dronePawn->GetVelocity();          
	const FRotator currRot = dronePawn->GetActorRotation();     
	const FRotator yawOnlyRot(0.f, currRot.Yaw, 0.f);  

	// Get world angular velocity and transform it to local frame using yaw-only rotation
	const FVector worldAngularRateDeg = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	localAngularRateDeg = yawOnlyRot.UnrotateVector(worldAngularRateDeg);

	if (bUseExternalController)
	{

		if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
		{
			Nav->UpdateNavigation(currPos);
			setPoint = Nav->GetCurrentSetpoint();
		}
        
		DrawDebugVisualsVel(FVector::ZeroVector);
        
		// Show UI if needed
		if (dronePawn && dronePawn->ImGuiUtil)
		{
			bool bShowUI = true;
			if (ADroneManager* Manager = ADroneManager::Get(dronePawn->GetWorld()))
			{
				if (!Manager->IsSwarmMode())
				{
					const int32 myIdx = Manager->GetDroneIndex(dronePawn);
					bShowUI = (myIdx == Manager->SelectedDroneIndex);
				}
			}
			if (bShowUI) { dronePawn->ImGuiUtil->ImGuiHud(currentFlightMode, DeltaTime); }
		}
        
		return; // Exit early - external controller handles everything else
	}
	
	// ───── Update / fetch next set-point ─────
	if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
	{
		Nav->UpdateNavigation(currPos);
		setPoint =Nav->GetCurrentSetpoint();
	}
	/*-------- Position P Control -------- */

	FVector desiredLocalVelocity;
	switch (currentFlightMode)
	{
	case EFlightMode::AutoWaypoint:
		{
			const FVector posErr = setPoint - currPos;
			const FVector localPosErr = yawOnlyRot.UnrotateVector(posErr);
			desiredLocalVelocity = localPosErr.GetSafeNormal()*maxVelocity;
			break;
		}
	case EFlightMode::VelocityControl:
		{
			desiredLocalVelocity = desiredNewVelocity;
			break;
		}
	case EFlightMode::AngleControl:
	case EFlightMode::RateControl:
		{
			desiredLocalVelocity = FVector(0.0f, 0.0f, desiredNewVelocity.Z);
			break;
		}
	default:
		desiredLocalVelocity = FVector::ZeroVector;
		break;
	}
	/*-------- Hover Move -------- */
	if (bHoverModeActive)
	{
		desiredLocalVelocity.Z = AltitudePID->Calculate(hoverTargetAltitude,currPos.Z, DeltaTime);
		desiredLocalVelocity.Z = FMath::Clamp(desiredLocalVelocity.Z, -100.f, 100.f);
	}
	
	/*-------- Velocity PID Control (FLU) -------- */ 
	currentLocalVelocity = yawOnlyRot.UnrotateVector(currVel);
	
	const double xOut = CurrentSet->XPID -> Calculate(desiredLocalVelocity.X,currentLocalVelocity.X, DeltaTime);
	const double yOut = CurrentSet->YPID -> Calculate(desiredLocalVelocity.Y,currentLocalVelocity.Y, DeltaTime);
	const double zOut = CurrentSet->ZPID -> Calculate(desiredLocalVelocity.Z,currentLocalVelocity.Z, DeltaTime);
	
	desiredRoll  = FMath::Clamp(yOut, -maxAngle,  maxAngle);
	desiredPitch = FMath::Clamp(xOut, -maxAngle,  maxAngle);
	desiredYawRate = 0.f; // Command a stable yaw rate, can be expanded later.

	// Calculate and store normalized thrust
	const float droneMass = dronePawn->GetMass();
	const float gravity = 980.0f;
	const float hoverThrustForce = droneMass * gravity;
	const float maxTotalThrust = 700.0f * 4; // From your mixer's clamp value

	float totalThrustForce = hoverThrustForce + zOut;
	desiredThrust_Normalized = FMath::Clamp(totalThrustForce / maxTotalThrust, 0.0f, 1.0f);
    
	
	
	/*-------- Angle P Control -------- */ 
	
	desiredRoll  = (currentFlightMode == EFlightMode::AngleControl) ? desiredNewRoll: FMath::Clamp( yOut, -maxAngle,  maxAngle);
	desiredPitch = (currentFlightMode == EFlightMode::AngleControl) ? desiredNewPitch: FMath::Clamp( xOut, -maxAngle,  maxAngle);
	
	const double rollOut  = CurrentSet ->RollPID->Calculate(desiredRoll,currRot.Roll , DeltaTime);
	const double pitchOut = CurrentSet ->PitchPID->Calculate(desiredPitch,-currRot.Pitch, DeltaTime);
	// UE_LOG(LogTemp, Warning, TEXT("Roll Output: %f deg/s"), rollOut);
	// UE_LOG(LogTemp, Warning, TEXT("Pitch Output: %f deg/s"), pitchOut);

	/*-------- Angle Rate PID Control -------- */ 

	// desiredPitchRate = (currentFlightMode == EFlightMode::RateControl) ? desiredNewPitchRate: FMath::Clamp(pitchOut, -maxAngleRate, maxAngleRate); // Implement switch here for angle control using DesiredNewPitchRate
	// desiredRollRate = (currentFlightMode == EFlightMode::RateControl) ? desiredNewRollRate: FMath::Clamp(rollOut, -maxAngleRate,  maxAngleRate); // Implement switch here for angle control 
	//
	// UE_LOG(LogTemp, Warning, TEXT("Desired Pitch Rate: %f deg/s"), desiredPitchRate);
	// UE_LOG(LogTemp, Warning, TEXT("Desired Roll Rate: %f deg/s"), desiredRollRate);
	// UE_LOG(LogTemp, Warning, TEXT("Current Pitch Angular Rate: %f deg/s"), localAngularRateDeg.Y);
	// UE_LOG(LogTemp, Warning, TEXT("Current Roll Angular Rate: %f deg/s"), localAngularRateDeg.X);
	//
	// const double rollRateOut  = CurrentSet->RollRatePID->Calculate(desiredRollRate,localAngularRateDeg.X,  DeltaTime);
	// const double pitchRateOut = CurrentSet->PitchRatePID->Calculate(desiredPitchRate,localAngularRateDeg.Y, DeltaTime);

	const float yawOutput = YawRateControl(DeltaTime);

	//  Mix & apply motor thrusts / torques
	ThrustMixer(desiredRoll, desiredPitch, zOut, rollOut, pitchOut, yawOutput);
	
	//  Debug drawing and on‑screen HUD (optional)
	DrawDebugVisualsVel(FVector(desiredLocalVelocity.X, desiredLocalVelocity.Y, 0.f));

	if (dronePawn && dronePawn->ImGuiUtil)
	{
		bool bShowUI = true;
		if (ADroneManager* Manager = ADroneManager::Get(dronePawn->GetWorld()))
		{
			if (!Manager->IsSwarmMode())
			{
				const int32 myIdx = Manager->GetDroneIndex(dronePawn);
				bShowUI = (myIdx == Manager->SelectedDroneIndex);
			}
		}

		if (bShowUI){dronePawn->ImGuiUtil->ImGuiHud(currentFlightMode,DeltaTime);}
	}
	
}

// ---------------------- Thrust Functions ------------------------

void UQuadDroneController::ThrustMixer(double xOut, double yOut, double zOut, double rollOutput, double pitchOutput, double yawOutput)
{
	float droneMass = dronePawn->DroneBody->GetMass();
	const float gravity = 980.0f;
	const float hoverThrust = (droneMass * gravity) / 4.0f; 
 
	float baseThrust = hoverThrust + zOut / 4.0f;
	baseThrust /= FMath::Cos(FMath::DegreesToRadians(FMath::Sqrt(FMath::Pow(xOut, 2) + FMath::Pow(yOut, 2))));
 
	Thrusts[0] = baseThrust - pitchOutput + rollOutput - yawOutput; //FL
	Thrusts[1] = baseThrust - pitchOutput - rollOutput + yawOutput; //FR
	Thrusts[2] = baseThrust + pitchOutput + rollOutput + yawOutput; //BL
	Thrusts[3] = baseThrust + pitchOutput - rollOutput - yawOutput; //BR
	
	// Paper is FR, BL, FL, BR
	for (int i = 0; i < Thrusts.Num(); i++)
	{
		Thrusts[i] = FMath::Clamp(Thrusts[i], 0.0f, 700.0f);
	}

	for (int i = 0; i < Thrusts.Num(); i++)
	{
		if (!dronePawn || !dronePawn->Thrusters.IsValidIndex(i))
			continue;
		double force = Thrusts[i];
		dronePawn->Thrusters[i]->ApplyForce(force);
	}
	double YawTorqueForce = 2.0f;
	FVector upVector = dronePawn->GetActorUpVector();
	FVector virtualTorque = upVector * yawOutput * YawTorqueForce;
	for (UThrusterComponent* Thruster : dronePawn->Thrusters)
	{
		if (Thruster)
		{
			Thruster->ApplyTorque(virtualTorque, true);
		}
	}

}

float  UQuadDroneController::YawRateControl(double DeltaTime)
{
	if (!dronePawn || !dronePawn->DroneBody) return 0.0f;
	
	FFullPIDSet* CurrentSet = &PIDSet;
	if (!CurrentSet) return 0.0f;
	
	FVector currentAngularVelocity = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	float currentYawRate = currentAngularVelocity.Z;

	float currentdesiredYawRate = desiredYawRate;
	float yawTorqueFeedback = CurrentSet->YawRatePID->Calculate(currentdesiredYawRate,currentYawRate, DeltaTime);
	return yawTorqueFeedback;
}

// ------ Reset Functions ------------------------

void UQuadDroneController::ResetPID()
{
	PIDSet.XPID      ->Reset();
	PIDSet.YPID      ->Reset();
	PIDSet.ZPID      ->Reset();
	PIDSet.RollPID   ->Reset();
	PIDSet.PitchPID  ->Reset();
	PIDSet.RollRatePID ->Reset();
	PIDSet.PitchRatePID->Reset();
	PIDSet.YawRatePID  ->Reset();
}

void UQuadDroneController::ResetDroneIntegral()
{
	FFullPIDSet* CurrentSet = &PIDSet;
	if (!CurrentSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetDroneIntegral: No PID set found for current flight mode %d"), (int32)currentFlightMode);
		return;
	}

	CurrentSet->XPID->ResetIntegral();
	CurrentSet->YPID->ResetIntegral();
	CurrentSet->ZPID->ResetIntegral();
	CurrentSet->RollPID->ResetIntegral();
	CurrentSet->PitchPID->ResetIntegral();
	CurrentSet->RollRatePID->ResetIntegral();
	CurrentSet->PitchRatePID->ResetIntegral();
	CurrentSet->YawRatePID->ResetIntegral();
}
void UQuadDroneController::ResetDroneRotation()
{
	if (dronePawn)
	{
		if (dronePawn->DroneBody)
		{
			dronePawn->DroneBody->SetSimulatePhysics(false);
		}

		dronePawn->SetActorRotation(FRotator::ZeroRotator);

		if (dronePawn->DroneBody)
		{
			dronePawn->DroneBody->SetSimulatePhysics(true);
			dronePawn->DroneBody->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dronePawn->DroneBody->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			dronePawn->DroneBody->WakeAllRigidBodies();
		}

		// Reset controller states
		ResetPID();
		desiredNewVelocity = FVector::ZeroVector;
	}
}
void UQuadDroneController::ResetDroneOrigin()
{
	if (dronePawn)
	{
		if (dronePawn->DroneBody)
		{
			dronePawn->DroneBody->SetSimulatePhysics(false);
		}

		dronePawn->SetActorLocation(FVector(0.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);
		dronePawn->SetActorRotation(FRotator::ZeroRotator);

		if (dronePawn->DroneBody)
		{
			dronePawn->DroneBody->SetSimulatePhysics(true);
			dronePawn->DroneBody->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dronePawn->DroneBody->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			dronePawn->DroneBody->WakeAllRigidBodies();
		}

		ResetPID();
		desiredNewVelocity = FVector::ZeroVector;
	}
}


// ------------ Setter and Getter -------------------


void UQuadDroneController::SetManualThrustMode(bool bEnable)
{
	bManualThrustMode = bEnable;
	if (bManualThrustMode)
	{	
		UE_LOG(LogTemp, Display, TEXT("Manual Thrust Mode ENABLED"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Manual Thrust Mode DISABLED"));
	}
}

void UQuadDroneController::SetHoverMode(bool bActive, float TargetAltitude)
{
	if (bActive && bHoverModeActive && dronePawn && TargetAltitude != hoverTargetAltitude)
	{
		hoverTargetAltitude = TargetAltitude;
		UE_LOG(LogTemp, Display, TEXT("Hover mode target altitude updated to: %.2f"), hoverTargetAltitude);
	}
	else if (bActive && !bHoverModeActive && dronePawn)
	{
		bHoverModeActive = true;
		hoverTargetAltitude = dronePawn->GetActorLocation().Z;
		AltitudePID->Reset();
		UE_LOG(LogTemp, Display, TEXT("Hover mode activated - Target altitude: %.2f"), hoverTargetAltitude);
	}
	else if (!bActive && bHoverModeActive)
	{
		bHoverModeActive = false;
		UE_LOG(LogTemp, Display, TEXT("Hover mode deactivated"));
	}
}

void UQuadDroneController::SetDestination(FVector desiredSetPoints) {
	setPoint = desiredSetPoints;
}


// ---------------------- Helper Functions -----------------------

void UQuadDroneController::DrawDebugVisuals(const FVector& currentPosition) const
{
   // Draw only a line connecting the current position to the setpoint (no spheres).
   DrawDebugLine(
       dronePawn->GetWorld(),
       currentPosition,
       setPoint,
       FColor::Green,
       /*bPersistent=*/false,
       /*LifeTime=*/0.0f
   );
}

 void UQuadDroneController::DrawDebugVisualsVel(const FVector& horizontalVelocity) const
 {
 	if (!bDebugVisualsEnabled || !dronePawn || !dronePawn->DroneBody) return;

 	FVector dronePos = dronePawn->GetActorLocation();
 	const float scaleXYZ = 0.5f;

 	// Velocity debug lines
 	DrawDebugLine(dronePawn->GetWorld(), dronePos, dronePos + FVector(desiredNewVelocity.X, 0, 0) * scaleXYZ, FColor::Red, false, -1.0f, 0, 2.0f);
 	DrawDebugLine(dronePawn->GetWorld(), dronePos, dronePos + FVector(0, desiredNewVelocity.Y, 0) * scaleXYZ, FColor::Green, false, -1.0f, 0, 2.0f);
 	DrawDebugLine(dronePawn->GetWorld(), dronePos, dronePos + FVector(0, 0, desiredNewVelocity.Z) * scaleXYZ, FColor::Blue, false, -1.0f, 0, 2.0f);
	
 	// Motor labels
 	for (int i = 0; i < dronePawn->Thrusters.Num(); i++) {
 		FVector MotorPos = dronePawn->Thrusters[i]->GetComponentLocation();
 		FString DirText = dronePawn->MotorClockwiseDirections[i] ? TEXT("CW") : TEXT("CCW");
 		DrawDebugString(GetWorld(), MotorPos + FVector(0, 0, 15),
 			FString::Printf(TEXT("M%d\n%s"), i, *DirText),
 			nullptr, FColor::White, 0.0f, true, 1.2f);
 	}
 }

void UQuadDroneController::SafetyReset()
{

	FRotator currentRotation = dronePawn->GetActorRotation();
	FVector currentPosition = dronePawn->GetActorLocation();

	// Safety check: if dangerous orientation, reset.
	bool needsReset = false;
	if (currentPosition.Z && (FMath::Abs(currentRotation.Roll) > 60.0f || FMath::Abs(currentRotation.Pitch) > 60.0f))
	{
		needsReset = true;
		UE_LOG(LogTemp, Warning, TEXT("Safety Reset: Dangerous orientation - Roll: %f, Pitch: %f"), currentRotation.Roll, currentRotation.Pitch);
	}
	if (needsReset)
	{
		if (dronePawn->DroneBody)
		{
			FRotator safeRotation = FRotator(0.0f, currentRotation.Yaw, 0.0f);
			dronePawn->SetActorRotation(safeRotation, ETeleportType::TeleportPhysics);
			dronePawn->DroneBody->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dronePawn->DroneBody->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			ResetDroneIntegral();
		}
	}
}
void UQuadDroneController::ApplyManualThrusts()
{
	if (!dronePawn)
		return;

	float droneMass = dronePawn->DroneBody->GetMass();
	const float mult = 0.5f;
	for (int i = 0; i < Thrusts.Num(); i++)
	{
		Thrusts[i] = FMath::Clamp(Thrusts[i], 0.0f, 700.0f);
		if (!dronePawn->Thrusters.IsValidIndex(i))
			continue;
		float force = droneMass * mult * Thrusts[i];
		dronePawn->Thrusters[i]->ApplyForce(force);
	}
}
float UQuadDroneController::GetCurrentThrustOutput(int32 ThrusterIndex) const
{
	if (Thrusts.IsValidIndex(ThrusterIndex))
	{
		return Thrusts[ThrusterIndex];
	}
	return 0.0f;
}
FVector UQuadDroneController::GetCurrentVelocity() const
{
	if (IsValid(dronePawn) && IsValid(dronePawn->DroneBody))
	{
		return dronePawn->DroneBody->GetPhysicsLinearVelocity();
	}
	return FVector::ZeroVector;
}
void UQuadDroneController::SetFlightMode(EFlightMode NewMode)
{
	switch (NewMode)
	{
	case EFlightMode::AngleControl:
	case EFlightMode::JoyStickAngleControl:
	case EFlightMode::JoyStickAcroControl:
		hoverTargetAltitude = dronePawn ? dronePawn->GetActorLocation().Z : 0.f;
		AltitudePID->Reset();                       // forget old integral wind-up
		break;

	default:
		break;
	}
    currentFlightMode = NewMode;
    // On selecting AutoWaypoint, generate and load the figure-8 navigation plan
    if (NewMode == EFlightMode::AutoWaypoint && dronePawn)
    {
        // Generate waypoints from pawn's position
        TArray<FVector> plan = dronePawn->GenerateFigureEightWaypoints();
        // Set navigation plan
        if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
        {
            Nav->SetNavigationPlan(plan);
        }
        // Debug draw the path: spheres and connecting lines
        UWorld* World = dronePawn->GetWorld();
        if (World)
        {
            // Persistent debug: sample path sparsely for performance
            const float SphereSize = 50.0f;
            const int32 debugStep = 5;
            const bool bPersistent = true;
            const float LifeTime = 0.0f;
            for (int32 i = 0; i < plan.Num(); i += debugStep)
            {
                // Draw only connecting lines for auto-waypoint path (no spheres)
                int32 nextIdx = i + debugStep;
                if (nextIdx < plan.Num())
                {
                    DrawDebugLine(World,
                        plan[i],
                        plan[nextIdx],
                        FColor::Green,
                        /*bPersistent=*/true,
                        /*LifeTime=*/0.0f,
                        /*DepthPriority=*/0,
                        /*Thickness=*/5.0f);
                }
            }
        }
    }
}


//=========================== PX4 Implementation =========================== //

void UQuadDroneController::ApplyMotorCommands(const TArray<float>& MotorCommands)
{
    if (!dronePawn || MotorCommands.Num() < 4)
    {
        UE_LOG(LogTemp, Warning, TEXT("ApplyMotorCommands: Invalid input - dronePawn=%p, MotorCommands.Num()=%d"), 
               dronePawn, MotorCommands.Num());
        return;
    }
    
    // Convert normalized motor commands (0.0-1.0) to thrust forces
    const float MaxThrust = 700.0f; // Your current max thrust value
    
    for (int32 i = 0; i < FMath::Min(MotorCommands.Num(), 4); i++)
    {
        if (dronePawn->Thrusters.IsValidIndex(i) && dronePawn->Thrusters[i])
        {
            float ThrustForce = FMath::Clamp(MotorCommands[i], 0.0f, 1.0f) * MaxThrust;
            dronePawn->Thrusters[i]->ApplyForce(ThrustForce);
            
            // Update Thrusts array for UI display
            if (Thrusts.IsValidIndex(i))
            {
                Thrusts[i] = ThrustForce;
            }
        }
    }
    
    UE_LOG(LogTemp, VeryVerbose, TEXT("Applied motor commands: [%.2f, %.2f, %.2f, %.2f] -> [%.0f, %.0f, %.0f, %.0f]N"), 
           MotorCommands[0], MotorCommands[1], MotorCommands[2], MotorCommands[3],
           MotorCommands[0] * MaxThrust, MotorCommands[1] * MaxThrust, 
           MotorCommands[2] * MaxThrust, MotorCommands[3] * MaxThrust);
}

void UQuadDroneController::SetUseExternalController(bool bUseExternal)
{
    if (bUseExternalController != bUseExternal)
    {
        bUseExternalController = bUseExternal;
        
        if (bUseExternal)
        {
            UE_LOG(LogTemp, Warning, TEXT("Switched to external controller mode (PX4)"));
            // Reset PIDs to avoid windup when switching back
            ResetPID();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Switched to internal controller mode"));
            // Reset PIDs when switching back to internal control
            ResetPID();
        }
    }
}

FVector UQuadDroneController::GetCurrentPosition() const
{
    if (dronePawn)
    {
        return dronePawn->GetActorLocation();
    }
    return FVector::ZeroVector;
}

FRotator UQuadDroneController::GetCurrentRotation() const
{
    if (dronePawn)
    {
        return dronePawn->GetActorRotation();
    }
    return FRotator::ZeroRotator;
}
