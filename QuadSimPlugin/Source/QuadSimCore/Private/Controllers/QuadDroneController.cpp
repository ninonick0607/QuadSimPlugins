
// QuadDroneController.cpp

#include "Controllers/QuadDroneController.h"
#include "Pawns/QuadPawn.h"
#include "DrawDebugHelpers.h"
#include "imgui.h"
#include "UI/ImGuiUtil.h"
#include "Core/DroneJSONConfig.h"
#include "Core/DroneManager.h"
#include "Core/DroneMathUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"


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
	ControllerSet.RollRatePID->SetGains(0,0,0);
	
	ControllerSet.PitchRatePID = new QuadPIDController();
	ControllerSet.PitchRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	ControllerSet.PitchRatePID->SetGains(1.0f,0,0);
	
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
			// Independent mode: only show UI on the possessed pawn
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PC && PC->GetPawn() != dronePawn)
			{
				bShowUI = false;
			}
		}
		else
		{
			// Swarm mode: only show UI on first drone (index 0)
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
		// Flight mode buttons
        if (ImGui::Button("Position Control", ImVec2(200, 50)))
        {
            // Switch to auto-waypoint mode and load figure-8 plan
            SetFlightMode(EFlightMode::AutoWaypoint);
            // Broadcast to swarm if enabled
            if (Manager && Manager->IsSwarmMode())
                Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        }
        // if (ImGui::Button("JoyStick Control", ImVec2(200, 50)))
        // {
        //     // Switch to joystick control mode
        //     SetFlightMode(EFlightMode::JoyStickControl);
        //     if (Manager && Manager->IsSwarmMode())
        //         Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        // }
        if (ImGui::Button("Move By Velocity", ImVec2(200, 50)))
        {
            // Switch to velocity control mode
            SetFlightMode(EFlightMode::VelocityControl);
            if (Manager && Manager->IsSwarmMode())
                Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        }
		if (ImGui::Button("Angle Control", ImVec2(200, 50)))
		{
			// Switch to joystick control mode
			SetFlightMode(EFlightMode::AngleControl);
			if (Manager && Manager->IsSwarmMode())
				Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
		}
		// if (ImGui::Button("Rate Control", ImVec2(200, 50)))
		// {
		// 	// Switch to velocity control mode
		// 	SetFlightMode(EFlightMode::RateControl);
		// 	if (Manager && Manager->IsSwarmMode())
		// 		Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
		// }
		ImGui::End();
	}

	if(currentFlightMode != EFlightMode::None){
		FlightController(a_deltaTime);
	}
}

void UQuadDroneController::FlightController(double DeltaTime)
{
	FFullPIDSet* CurrentSet = &PIDSet;
	if (!CurrentSet || !dronePawn) return;
	
	/* ───── World-space state ───── */
	const FVector  currPos = dronePawn->GetActorLocation();     // cm
	const FVector  currVel =	 dronePawn->GetVelocity();          // cm/s
	const FRotator currRot = dronePawn->GetActorRotation();     // deg   (Unreal FRU)
	const FRotator yawOnlyRot(0.f, currRot.Yaw, 0.f);  // inverse yaw helper

	const FVector worldAngularRateDeg = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	localAngularRateDeg = yawOnlyRot.UnrotateVector(worldAngularRateDeg);
	
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
			desiredLocalVelocity = DroneMathUtils::CalculateDesiredVelocity(localPosErr, maxVelocity);
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

	/*-------- Angle P Control -------- */ 
	
	desiredRoll  = (currentFlightMode == EFlightMode::AngleControl) ? desiredNewRoll: FMath::Clamp( yOut, -maxAngle,  maxAngle); // Implement switch here for angle control using DesirednewRoll
	desiredPitch = (currentFlightMode == EFlightMode::AngleControl) ? desiredNewPitch: FMath::Clamp( xOut, -maxAngle,  maxAngle); // Implement switch here
	
	const double rollOut  = CurrentSet ->RollPID->Calculate(desiredRoll,currRot.Roll , DeltaTime);
	const double pitchOut = CurrentSet ->PitchPID->Calculate(desiredPitch,-currRot.Pitch, DeltaTime);
	// UE_LOG(LogTemp, Warning, TEXT("Roll Output: %f deg/s"), rollOut);
	// UE_LOG(LogTemp, Warning, TEXT("Pitch Output: %f deg/s"), pitchOut);

	/*-------- Angle Rate PID Control -------- */ 

	// desiredPitchRate = (currentFlightMode == EFlightMode::RateControl) ? desiredNewPitchRate: FMath::Clamp(pitchOut, -maxAngleRate, maxAngleRate); // Implement switch here for angle control using DesiredNewPitchRate
	// desiredRollRate = (currentFlightMode == EFlightMode::RateControl) ? desiredNewRollRate: FMath::Clamp(rollOut, -maxAngleRate,  maxAngleRate); // Implement switch here for angle control 

	// UE_LOG(LogTemp, Warning, TEXT("Desired Pitch Rate: %f deg/s"), desiredPitchRate);
	// UE_LOG(LogTemp, Warning, TEXT("Desired Roll Rate: %f deg/s"), desiredRollRate);
	// UE_LOG(LogTemp, Warning, TEXT("Current Pitch Angular Rate: %f deg/s"), localAngularRateDeg.Y);
	// UE_LOG(LogTemp, Warning, TEXT("Current Roll Angular Rate: %f deg/s"), localAngularRateDeg.X);

	// const double pitchRateOut = CurrentSet->PitchRatePID->Calculate(desiredPitchRate,-localAngularRateDeg.Y, DeltaTime);
	// const double rollRateOut  = CurrentSet->RollRatePID->Calculate(desiredRollRate,localAngularRateDeg.X,  DeltaTime);

	const float yawOutput = YawRateControl(DeltaTime);

	//  Mix & apply motor thrusts / torques
	ThrustMixer(desiredRoll, desiredPitch, zOut, rollOut, pitchOut, yawOutput);

	//  Debug drawing and on‑screen HUD (optional)
	DrawDebugVisualsVel(FVector(desiredLocalVelocity.X, desiredLocalVelocity.Y, 0.f));

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
void UQuadDroneController::dynamicController(double DeltaTime)
{
	FFullPIDSet* CurrentSet = &PIDSet;


	// Drone Config
	float droneMass = 2.f;
	float gravity = 980; // cm/s^2
	float hoverThrust =  droneMass*gravity;

	float motorArm_x_m = 23; // cm
	float motorArm_y_m = 23; // cm
	float armLength_m = FMath::Sqrt(FMath::Square(motorArm_x_m)+FMath::Square(motorArm_y_m));
	float angleRad = FMath::Atan2(motorArm_y_m,motorArm_x_m);
	float armAngleSin = FMath::Sin(angleRad);
	float armAngleCos = FMath::Cos(angleRad);
	
	float thrustCoeff =8.54858e-06;
	float torqueCoeff = 0.06;
	float torqueToThrust = thrustCoeff / torqueCoeff;
	float maxMotorSpeed = 1100.f; // Rad/s
	float maxThrust = thrustCoeff * FMath::Square(maxMotorSpeed);

	float maxTotalThrust = maxThrust * 4;
	float maxThrustMotor = maxTotalThrust/4;
	
	float motorSpeed = FMath::Sqrt(1/torqueCoeff); // Gives me angular velocity for each propeller (Needed for correct rotation)
	
	// End of Drone Config

	/* ───── World-space state ───── */
	const FVector  currPos = dronePawn->GetActorLocation();     // cm
	const FVector  currVel = dronePawn->GetVelocity();          // cm/s
	const FRotator currRot = dronePawn->GetActorRotation();     // deg   (Unreal FRU)
	const FRotator yawOnlyRot(0.f, currRot.Yaw, 0.f);  // inverse yaw helper

	// ───── Update / fetch next set-point ─────
	if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
	{
		Nav->UpdateNavigation(currPos);
		setPoint = { 500,500,500 };//Nav->GetCurrentSetpoint();
	}
	/*-------- Position P Control -------- */

	const FVector posErr = setPoint - currPos;
	const FVector localPosErr = yawOnlyRot.UnrotateVector(posErr);

	float Kp = 1.f;
	FVector desiredLocalVelocity = (currentFlightMode == EFlightMode::VelocityControl) ? desiredNewVelocity: DroneMathUtils::CalculateDesiredVelocity(localPosErr, maxVelocity);
	
	/*-------- Hover Move -------- */
	if (bHoverModeActive && currentFlightMode == EFlightMode::VelocityControl)
	{
		desiredLocalVelocity.Z = AltitudePID->Calculate(hoverTargetAltitude,currPos.Z, DeltaTime); // cm/s
		desiredLocalVelocity.Z = FMath::Clamp(desiredLocalVelocity.Z, -100.f, 100.f);
	}
	
	/*-------- Velocity PID Control (FLU) -------- */ 
	currentLocalVelocity = yawOnlyRot.UnrotateVector(currVel);

	const double xOut = CurrentSet->XPID -> Calculate(desiredLocalVelocity.X,currentLocalVelocity.X, DeltaTime);
	const double yOut = CurrentSet->YPID -> Calculate(desiredLocalVelocity.Y,currentLocalVelocity.Y, DeltaTime);
	const double zOut = CurrentSet->ZPID -> Calculate(desiredLocalVelocity.Z,currentLocalVelocity.Z, DeltaTime);

	/*-------- Angle P Control -------- */ 

	desiredRoll  = FMath::Clamp( yOut, -maxAngle,  maxAngle);
	desiredPitch = FMath::Clamp( xOut, -maxAngle,  maxAngle);
	
	const double rollOut  = CurrentSet ->RollPID->Calculate(desiredRoll,currRot.Roll , DeltaTime);
	const double pitchOut = CurrentSet ->PitchPID->Calculate(desiredPitch,-currRot.Pitch, DeltaTime);
	
	float baseThrust = hoverThrust + zOut / 4.0f;
	baseThrust /= FMath::Cos(FMath::DegreesToRadians(FMath::Sqrt(FMath::Pow(xOut, 2) + FMath::Pow(yOut, 2))));
	
	float pitchMomentArm = 1;//armLength_m*armAngleSin;
	float rollMomentArm = 1;//armLength_m*armAngleCos;
	
	Thrusts[0] = (baseThrust - pitchOut/pitchMomentArm + rollOut/rollMomentArm)/4; // FL
	Thrusts[1] = (baseThrust - pitchOut/pitchMomentArm - rollOut/rollMomentArm)/4; // FR
	Thrusts[2] = (baseThrust + pitchOut/pitchMomentArm + rollOut/rollMomentArm)/4; // BL
	Thrusts[3] = (baseThrust + pitchOut/pitchMomentArm - rollOut/rollMomentArm)/4; // BR
	
	for (int i = 0; i < Thrusts.Num(); i++)
	{
		Thrusts[i] = FMath::Clamp(Thrusts[i], 0.0f, 700);
	}

	for (int i = 0; i < Thrusts.Num(); i++)
	{
		if (!dronePawn || !dronePawn->Thrusters.IsValidIndex(i))
			continue;
		double force = Thrusts[i];
		dronePawn->Thrusters[i]->ApplyForce(force);
	}
	
	DrawDebugVisualsVel(FVector(desiredLocalVelocity.X, desiredLocalVelocity.Y, 0));
	
	// UI: only show for possessed (independent) or first drone in swarm
	if (dronePawn && dronePawn->ImGuiUtil)
	{
		// Show UI for the drone at the selected index in the manager
		ADroneManager* Manager = ADroneManager::Get(dronePawn->GetWorld());
		bool bShowUI = true;
		if (Manager)
		{
			if (!Manager->IsSwarmMode())
			{
				int32 MyIndex = Manager->GetDroneIndex(dronePawn);
				bShowUI = (MyIndex == Manager->SelectedDroneIndex);
			}
			else
			{
				// In swarm mode, show UI on all drones
				bShowUI = true;
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

	float yawTorqueFeedback = CurrentSet->YawRatePID->Calculate(desiredYawRate,currentYawRate, DeltaTime);
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

FQuat UQuadDroneController::GetOrientationAsQuat() const 
{
	if (IsValid(dronePawn))
	{
		const FRotator WorldRotation = dronePawn->GetActorRotation();
		return FQuat(WorldRotation); 
	}
	return FQuat::Identity; 
}

FVector UQuadDroneController::GetCurrentAngularVelocityRADPS() const
{
	if (IsValid(dronePawn) && IsValid(dronePawn->DroneBody))
	{
		FVector AngularVelocityDegS = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
		return FVector(
			FMath::DegreesToRadians(AngularVelocityDegS.X),
			FMath::DegreesToRadians(AngularVelocityDegS.Y),
			FMath::DegreesToRadians(AngularVelocityDegS.Z)
		);
	}
	UE_LOG(LogTemp, Warning, TEXT("GetCurrentAngularVelocityRADPS: dronePawn or DroneBody invalid. Returning zero vector."));
	return FVector::ZeroVector;
}
