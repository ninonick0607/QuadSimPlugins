
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
	, desiredYaw(0.f)
	, currentLocalVelocity(FVector::ZeroVector)
	, desiredForwardVector(FVector(1.0f, 0.0f, 0.0f))
	, YawTorqueForce(2.0)
	, LastYawTorqueApplied(0.0)
	, desiredYawRate(0.0f) 
	, bDebugVisualsEnabled(false)
	, setPoint(FVector::ZeroVector)
	, desiredNewVelocity(FVector::ZeroVector)
	, hoverTargetAltitude(0.0f)
	, bHoverModeActive(false)
	, bManualThrustMode(false)  
{
	const auto& Config = UDroneJSONConfig::Get().Config;
	maxVelocity = Config.FlightParams.MaxVelocity;
	maxAngle = Config.FlightParams.MaxAngle;
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
	
	FFullPIDSet VelocitySet;

	VelocitySet.XPID = new QuadPIDController();
	VelocitySet.XPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.XPID->SetGains(0.03f, 0.f, 0.0f);

	VelocitySet.YPID = new QuadPIDController();
	VelocitySet.YPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.YPID->SetGains(0.03f, 0.0f, 0.0f);

	VelocitySet.ZPID = new QuadPIDController();
	VelocitySet.ZPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.ZPID->SetGains(5.f, 0.0f, 0.0f);

	VelocitySet.RollPID = new QuadPIDController();
	VelocitySet.RollPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.RollPID->SetGains(0.31f, 0.2f, 0.34f);

	VelocitySet.PitchPID = new QuadPIDController();
	VelocitySet.PitchPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.PitchPID->SetGains(0.35f, 0.16f, 0.25f);

	VelocitySet.RollRatePID = new QuadPIDController();
	VelocitySet.RollRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.RollRatePID->SetGains(0.31f, 0.2f, 0.34f);
	
	VelocitySet.PitchRatePID = new QuadPIDController();
	VelocitySet.PitchRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.PitchRatePID->SetGains(0.31f, 0.2f, 0.34f);
	
	VelocitySet.YawRatePID = new QuadPIDController();
	VelocitySet.YawRatePID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.YawRatePID->SetGains(1.0f, 0.0f, 0.0f);
//--------------------------------------------------------------

	PIDMap.Add(EFlightMode::VelocityControl, MoveTemp(VelocitySet));
	
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
        if (ImGui::Button("Auto Waypoint", ImVec2(200, 50)))
        {
            // Switch to auto-waypoint mode and load figure-8 plan
            SetFlightMode(EFlightMode::AutoWaypoint);
            // Broadcast to swarm if enabled
            if (Manager && Manager->IsSwarmMode())
                Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        }
        if (ImGui::Button("JoyStick Control", ImVec2(200, 50)))
        {
            // Switch to joystick control mode
            SetFlightMode(EFlightMode::JoyStickControl);
            if (Manager && Manager->IsSwarmMode())
                Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        }
        if (ImGui::Button("Move By Velocity", ImVec2(200, 50)))
        {
            // Switch to velocity control mode
            SetFlightMode(EFlightMode::VelocityControl);
            if (Manager && Manager->IsSwarmMode())
                Manager->OnGlobalFlightModeChanged.Broadcast(currentFlightMode);
        }
		ImGui::End();
	}
	
	switch (currentFlightMode)
	{
	case EFlightMode::None:
		return;
	case EFlightMode::AutoWaypoint:
		AutoWaypointControl(a_deltaTime);
		break;
	case EFlightMode::JoyStickControl:
		//ApplyControllerInput(a_deltaTime);
		break;
	case EFlightMode::VelocityControl:
		FlightController(a_deltaTime);
		break;
	}
	
}

// Velocity‑control inner‑loop for QuadRotor
// ------------------------------------------------------------
// Notes on conventions:
//   • All vectors returned by UE are **world‑space**.
//   • We do control in the **body‑local** X (forward) / Y (right) / Z (up) frame.
//   • Yaw is separated: we rotate world vectors by the current yaw only so that
//     roll & pitch commands are not affected by the drone’s heading.
//   • Translational PIDs (XPID,YPID ZPID) work on **velocity error** in m/s.
//   • Attitude PIDs (RollPID,PitchPID) work on **angle error** in degrees.
//   • ThrustMixer maps desired forces/torques → 4 individual motor thrusts.
// ------------------------------------------------------------

void UQuadDroneController::VelocityControl(double DeltaTime)
{
    //  Fetch the active PID set for this flight‑mode
    FFullPIDSet* CurrentSet = PIDMap.Find(EFlightMode::VelocityControl);
    if (!CurrentSet || !dronePawn) return;

    //  Read current state (world‑space)
    const FVector   worldPos  = dronePawn->GetActorLocation();   // cm
    const FVector   worldVel  = dronePawn->GetVelocity();        // cm/s
    const FRotator  worldRot  = (dronePawn->GetActorRotation());   // deg

    //  Build **desired local velocity**
    FVector desiredLocalVel = desiredNewVelocity; // set by external caller (body frame)

    // If hover‑hold is enabled, Z velocity is produced by a dedicated Altitude PID
    if (bHoverModeActive)
    {
        const float altitudeError   = hoverTargetAltitude - worldPos.Z;      //cm
        desiredLocalVel.Z = AltitudePID->Calculate(altitudeError, DeltaTime); // cm/s
        desiredLocalVel.Z = FMath::Clamp(desiredLocalVel.Z, -100.f, 100.f);
    }

    //------------------------------------------------------------
    //  Transform current velocity into the **body‑local** frame
    //    (rotate world velocity by –Yaw so X = forward, Y = right)
    //------------------------------------------------------------
	const FRotator yawOnlyRot(0.f, worldRot.Yaw, 0.f);
    currentLocalVelocity = yawOnlyRot.UnrotateVector(worldVel); // cm/s in body frame

    //  Compute velocity error in the local frame
    const FVector velError = desiredLocalVel - currentLocalVelocity; // cm/s
	
    //  Optional safety reset (limits max attitude, etc.)
    SafetyReset();

    //  Translational PIDs  (output: desired roll/pitch angles & Z force
    double xOut = CurrentSet->XPID->Calculate(velError.X, DeltaTime); // desired pitch (deg)
    double yOut = CurrentSet->YPID->Calculate(velError.Y, DeltaTime); // desired roll  (deg)
    double zOut = CurrentSet->ZPID->Calculate(velError.Z, DeltaTime); // desired Z force component

    // Clamp roll & pitch commands to physical max angle
    yOut = ( FMath::Clamp(yOut, -maxAngle, maxAngle)); // rad
    xOut = (FMath::Clamp(xOut, -maxAngle, maxAngle)); // rad

    //  Attitude PIDs  (output: torque commands)
    const double rollError  = yOut - worldRot.Roll;   // rad
    const double pitchError = xOut - (-worldRot.Pitch);  // rad

    const double rollOut  = CurrentSet->RollPID ->Calculate(rollError , DeltaTime); // Nm (abstract)
    const double pitchOut = CurrentSet->PitchPID->Calculate(pitchError, DeltaTime); // Nm (abstract)

    //  Mix & apply motor thrusts / torques
    ThrustMixer(xOut, yOut, zOut, rollOut, pitchOut);

    //  Yaw is controlled as a separate outer/inner loop elsewhere
    YawRateControl(DeltaTime);

    //  Debug drawing and on‑screen HUD (optional)
    DrawDebugVisualsVel(FVector(desiredLocalVel.X, desiredLocalVel.Y, 0.f));

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

        if (bShowUI)
        {
            dronePawn->ImGuiUtil->ImGuiHud(
                currentFlightMode,
                Thrusts,
                /*yCmd =*/ yOut,
                /*xCmd =*/ xOut,
                worldRot,
                /*unused accel*/ FVector::ZeroVector,
                worldPos,
                /*unused target*/ FVector::ZeroVector,
                currentLocalVelocity,
                maxVelocity,
                maxAngle,
                /*pidOutputs*/ xOut, yOut, zOut,
                DeltaTime);
        }
    }
}
void UQuadDroneController::AutoWaypointControl(double DeltaTime)
{
    FFullPIDSet* CurrentSet = PIDMap.Find(EFlightMode::VelocityControl);
    if (!CurrentSet || !dronePawn) return;

    const FVector  currentPos      = dronePawn->GetActorLocation(); //cm
    const FVector  worldVel        = dronePawn->GetVelocity(); // cm/s
    const FRotator currentRot      = dronePawn->GetActorRotation(); // degrees
    const FRotator yawOnlyRot      (0.f, currentRot.Yaw, 0.f);

    // ───── Update / fetch next set-point ─────
    if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
    {
        Nav->UpdateNavigation(currentPos);
        setPoint = Nav->GetCurrentSetpoint();
    }

    /*---------------- LOCAL FRAME ----------------*/
    const FVector worldPosErr   = setPoint - currentPos;
    const FVector localPosErr   = yawOnlyRot.UnrotateVector(worldPosErr);  
    currentLocalVelocity        = yawOnlyRot.UnrotateVector(worldVel);     

    // Desired velocity in **local** frame
    const FVector desiredLocalVelocity =
        DroneMathUtils::CalculateDesiredVelocity(localPosErr, maxVelocity); // <-- use local error

    // Velocity error also in local frame
    const FVector velErrLocal = desiredLocalVelocity - currentLocalVelocity;

    /*-------------- PID TRANSLATIONAL -------------*/
    const double xOut = CurrentSet->XPID  ->Calculate(velErrLocal.X, DeltaTime);
    const double yOut = CurrentSet->YPID  ->Calculate(velErrLocal.Y, DeltaTime);
    const double zOut = CurrentSet->ZPID  ->Calculate(velErrLocal.Z, DeltaTime);

    /*-------------- PID ATTITUDE ------------------*/
    double rollCmd  = FMath::Clamp( yOut, -maxAngle,  maxAngle);
    double pitchCmd = FMath::Clamp( xOut, -maxAngle,  maxAngle);

    const double rollErr  = rollCmd  - currentRot.Roll;
    const double pitchErr = pitchCmd - currentRot.Pitch;

    const double rollOut  = CurrentSet->RollPID->Calculate(rollErr , DeltaTime);
    const double pitchOut = CurrentSet->PitchPID->Calculate(pitchErr, DeltaTime);

    /*-------------- Mix / Apply -------------------*/
    ThrustMixer(rollCmd, pitchCmd, zOut, rollOut, pitchOut);

    /*-------------- Cascaded yaw (unchanged) ------*/
    {
        if (worldVel.Size() > minVelocityForYaw)
        {
            const FVector localErrYaw = localPosErr; // already in local frame
            const float   yawErrRad   = FMath::Atan2(localErrYaw.Y, localErrYaw.X);
            const float   yawErrDeg   = FMath::RadiansToDegrees(yawErrRad);

            const float rawRate = CurrentSet->YawRatePID->Calculate(yawErrDeg, DeltaTime);
            desiredYawRate      = FMath::Clamp(rawRate, -maxYawRate, maxYawRate);
        }
        else
        {
            desiredYawRate = 0.f;
        }
    }
   // Apply yaw rate control to generate torque
   YawRateControl(DeltaTime);
     
	// UI: show only for independent possessed drone or first in swarm
	if (dronePawn && dronePawn->ImGuiUtil)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ADroneManager* Manager = ADroneManager::Get(dronePawn->GetWorld());
		bool bSwarm = Manager && Manager->IsSwarmMode();
		bool bPossessed = PC && PC->GetPawn() == dronePawn;
		bool bShowUI = true;
		if (Manager)
		{
			if (!bSwarm)
				bShowUI = bPossessed;
			else
				bShowUI = (Manager->GetDroneIndex(dronePawn) == 0);
		}
		if (bShowUI)
		{
			dronePawn->ImGuiUtil->ImGuiHud(currentFlightMode, Thrusts, rollErr, pitchErr, currentRot,
				FVector::ZeroVector, currentPos, FVector::ZeroVector, currentLocalVelocity,
				maxVelocity, maxAngle, xOut, yOut, zOut, DeltaTime);
		}
	}
}
void UQuadDroneController::FlightController(double DeltaTime)
{
	FFullPIDSet* CurrentSet = PIDMap.Find(EFlightMode::VelocityControl);
	if (!CurrentSet || !dronePawn) return;
	
	UPrimitiveComponent* DroneBody = Cast<UPrimitiveComponent>(dronePawn->GetRootComponent());
	if (!DroneBody) return;

	/* ───── World-space state ───── */
	const FVector  currPos = dronePawn->GetActorLocation();     // cm
	const FVector  currVel = dronePawn->GetVelocity();          // cm/s
	const FRotator currRot = dronePawn->GetActorRotation();     // deg   (Unreal FRU)
	const FRotator yawOnlyRot(0.f, currRot.Yaw, 0.f);  // inverse yaw helper
	
	const FVector worldAngularRateDeg = DroneBody->GetPhysicsAngularVelocityInDegrees();
	const FVector localAngularRateDeg = currRot.UnrotateVector(worldAngularRateDeg);

	// ───── Update / fetch next set-point ─────
	if (UNavigationComponent* Nav = dronePawn->FindComponentByClass<UNavigationComponent>())
	{
		Nav->UpdateNavigation(currPos);
		setPoint = {500,500,500};//Nav->GetCurrentSetpoint();
	}
	
	/*-------- Position P Control -------- */ 

	const FVector posErr = setPoint-currPos;
	const FVector localPosErr = yawOnlyRot.UnrotateVector(posErr);

	float Kp = 1.f;

	const FVector desiredLocalVelocity = desiredNewVelocity;//DroneMathUtils::CalculateDesiredVelocity(localPosErr, maxVelocity); // <-- use local error	

	//const FVector desiredLocalVelocity = localPosErr * Kp;
	
	/*-------- Velocity PID Control (FLU) -------- */ 
	currentLocalVelocity = yawOnlyRot.UnrotateVector(currVel);
	const FVector localVelErr =  desiredLocalVelocity - currentLocalVelocity;

	const double xOut = CurrentSet->XPID ->Calculate(localVelErr.X, DeltaTime);
	const double yOut = CurrentSet->YPID ->Calculate(localVelErr.Y, DeltaTime);
	const double zOut = CurrentSet->ZPID ->Calculate(localVelErr.Z, DeltaTime);

	/*-------- Angle P Control -------- */ 

	double rollCmd  = FMath::Clamp( yOut, -maxAngle,  maxAngle);
	double pitchCmd = FMath::Clamp( xOut, -maxAngle,  maxAngle);

	const double rollErr = rollCmd  - currRot.Roll;
	const double pitchErr = pitchCmd - (-currRot.Pitch); 

	const double rollOut  = CurrentSet ->RollPID->Calculate(rollErr , DeltaTime);
	const double pitchOut = CurrentSet ->PitchPID->Calculate(pitchErr, DeltaTime);

	/*-------- Angle Rate PID Control -------- */ 

	// const double rateRollErr = rollOut-localAngularRateDeg.X;
	// const double ratePitchErr = pitchOut-(-localAngularRateDeg.Y);
	//
	// const double rollRateOut  = CurrentSet->RollRatePID->Calculate(rateRollErr,  DeltaTime);
	// const double pitchRateOut = CurrentSet->PitchRatePID->Calculate(ratePitchErr, DeltaTime);

	//  Mix & apply motor thrusts / torques
	ThrustMixer(rollCmd, pitchCmd, zOut, rollOut, pitchOut);

	//  Yaw is controlled as a separate outer/inner loop elsewhere
	//YawRateControl(DeltaTime);

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

		if (bShowUI)
		{
			dronePawn->ImGuiUtil->ImGuiHud(
				currentFlightMode,
				Thrusts,
				/*yCmd =*/ yOut,
				/*xCmd =*/ xOut,
				currRot,
				/*unused accel*/ FVector::ZeroVector,
				currPos,
				/*unused target*/ FVector::ZeroVector,
				currentLocalVelocity,
				maxVelocity,
				maxAngle,
				/*pidOutputs*/ xOut, yOut, zOut,
				DeltaTime);
		}
	}
	
}
void UQuadDroneController::dynamicController(double DeltaTime)
{
	FFullPIDSet* CurrentSet = PIDMap.Find(EFlightMode::VelocityControl);
	
	FVector currentPosition = dronePawn->GetActorLocation(); //cm 
	FVector currentVelocity = dronePawn->GetVelocity(); //cm/s
	FRotator currentRotation =FMath::DegreesToRadians(dronePawn->GetActorRotation()); //degrees
	FVector desiredLocalVelocity = desiredNewVelocity; // cm/s

	// Drone Config
	float droneMass = 2.f;
	float gravity = 980; // cm/s^2
	float hover_thrust =  droneMass*gravity;

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

	float maxAngleI = (15.0);
	//float maxYawRate = 45;
	
	// End of Drone Config
	
	if (bHoverModeActive)
	{
		float currentAltitude = dronePawn->GetActorLocation().Z;
		float altitudeError = hoverTargetAltitude - currentAltitude;
		desiredLocalVelocity.Z = AltitudePID->Calculate(altitudeError, DeltaTime);
		desiredLocalVelocity.Z = FMath::Clamp(desiredLocalVelocity.Z, -100.0f, 100.0f);
	}
 
	FRotator yawOnlyRotation(0, currentRotation.Yaw, 0);
	FVector currentLocalVelocityI = yawOnlyRotation.UnrotateVector(currentVelocity);
	FVector velocityError = desiredLocalVelocity - currentLocalVelocityI;
	SafetyReset();
	
	double x_output = 0.f, y_output = 0.f, z_output = 0.f;
	double roll_output = 0.f, pitch_output = 0.f, yaw_output = 0.f;

	x_output = CurrentSet->XPID->Calculate(velocityError.X, DeltaTime);
	y_output = CurrentSet->YPID->Calculate(velocityError.Y, DeltaTime);
	z_output = CurrentSet->ZPID->Calculate(velocityError.Z, DeltaTime);
	
	y_output = FMath::DegreesToRadians(FMath::Clamp(y_output, -maxAngleI, maxAngleI));
	float roll_error = y_output-currentRotation.Roll;
	roll_output = CurrentSet->RollPID->Calculate(roll_error, DeltaTime);

	x_output = FMath::DegreesToRadians(FMath::Clamp(x_output, -maxAngleI, maxAngleI));
	float pitch_error = x_output-(-currentRotation.Pitch);
	pitch_output = CurrentSet->PitchPID->Calculate(pitch_error, DeltaTime);

	float altitudeOutput = z_output+hover_thrust;

	const float hoverThrust = (droneMass * gravity); 
 
	float baseThrust = hoverThrust + z_output / 4.0f;
	baseThrust /= FMath::Cos((FMath::Sqrt(FMath::Pow(x_output, 2) + FMath::Pow(y_output, 2))));
	
	float pitchMomentArm = armLength_m*armAngleSin;
	float rollMomentArm = armLength_m*armAngleCos;
	
	Thrusts[0] = (baseThrust - pitch_output/pitchMomentArm + roll_output/rollMomentArm)/4; // FL
	Thrusts[1] = (baseThrust - pitch_output/pitchMomentArm - roll_output/rollMomentArm)/4; // FR
	Thrusts[2] = (baseThrust + pitch_output/pitchMomentArm + roll_output/rollMomentArm)/4; // BL
	Thrusts[3] = (baseThrust + pitch_output/pitchMomentArm - roll_output/rollMomentArm)/4; // BR

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
		if (bShowUI)
		{
			dronePawn->ImGuiUtil->ImGuiHud(currentFlightMode, Thrusts, y_output, x_output, currentRotation,
				FVector::ZeroVector, currentPosition, FVector::ZeroVector, currentLocalVelocity,
				maxVelocity, maxAngleI, x_output, y_output, z_output, DeltaTime);
		}
	}
}
// ---------------------- Thrust Functions ------------------------

void UQuadDroneController::ThrustMixer(double xOut, double yOut, double zOut, double rollOutput, double pitchOutput)
{
	float droneMass = dronePawn->DroneBody->GetMass();
	const float gravity = 980.0f;
	const float hoverThrust = (droneMass * gravity) / 4.0f; 
 
	float baseThrust = hoverThrust + zOut / 4.0f;
	baseThrust /= FMath::Cos(FMath::DegreesToRadians(FMath::Sqrt(FMath::Pow(xOut, 2) + FMath::Pow(yOut, 2))));
 
	Thrusts[1] = baseThrust - pitchOutput - rollOutput; //FR
	Thrusts[2] = baseThrust + pitchOutput + rollOutput; //BL
	Thrusts[0] = baseThrust - pitchOutput + rollOutput; //FL
	Thrusts[3] = baseThrust + pitchOutput - rollOutput; //BR
	
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
}

void UQuadDroneController::YawRateControl(double DeltaTime)
{
	if (!dronePawn || !dronePawn->DroneBody) return;

	FVector currentAngularVelocity = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	float currentYawRate = currentAngularVelocity.Z;

	float yawRateError = desiredYawRate - currentYawRate;

	FFullPIDSet* CurrentSet = GetPIDSet(currentFlightMode);
	if (!CurrentSet) return;

	float yawTorqueFeedback = CurrentSet->YawRatePID->Calculate(yawRateError, DeltaTime);

	float finalYawTorque = yawTorqueFeedback;

	FVector upVector = dronePawn->DroneBody->GetUpVector();
	FVector torqueVector = upVector * finalYawTorque * YawTorqueForce;
	LastYawTorqueApplied = finalYawTorque * YawTorqueForce;

	for (UThrusterComponent* Thruster : dronePawn->Thrusters)
	{
		if (Thruster)
		{
			Thruster->ApplyTorque(torqueVector, true);
		}
	}
}

// ------ Reset Functions ------------------------

void UQuadDroneController::ResetPID()
{
	for (auto& Elem : PIDMap)
	{
		FFullPIDSet& ThisSet = Elem.Value;

		ThisSet.XPID->Reset();
		ThisSet.YPID->Reset();
		ThisSet.ZPID->Reset();
		ThisSet.RollPID->Reset();
		ThisSet.PitchPID->Reset();
		ThisSet.RollRatePID->Reset();
		ThisSet.PitchRatePID->Reset();
		ThisSet.YawRatePID->Reset();
	}
}
void UQuadDroneController::ResetDroneIntegral()
{
	FFullPIDSet* CurrentSet = PIDMap.Find(currentFlightMode);
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
void UQuadDroneController::ResetDroneHigh()
{
	if (dronePawn)
	{
		if (dronePawn->DroneBody)
		{
			dronePawn->DroneBody->SetSimulatePhysics(false);
		}

		dronePawn->SetActorLocation(FVector(0.0f, 0.0f, 10000.0f), false, nullptr, ETeleportType::TeleportPhysics);
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
		desiredYaw = 0.0f;
		desiredForwardVector = FVector(1.0f, 0.0f, 0.0f);
	}
}


// ------------ Setter and Getter -------------------


void UQuadDroneController::SetDesiredVelocity(const FVector& NewVelocity)
{
	desiredNewVelocity = NewVelocity;
}

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

 	// Orientation arrows
 	FVector CurrentForward = dronePawn->GetActorForwardVector();
 	FVector DesiredForward = desiredForwardVector.GetSafeNormal();
 	DrawDebugDirectionalArrow(GetWorld(), dronePos, dronePos + CurrentForward * 200.f, 50.f, FColor::Red, false, -1.f, 0, 3.f);
 	DrawDebugDirectionalArrow(GetWorld(), dronePos, dronePos + DesiredForward * 200.f, 50.f, FColor::Cyan, false, -1.f, 0, 3.f);

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