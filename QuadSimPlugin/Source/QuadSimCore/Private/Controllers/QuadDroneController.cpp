
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
    FFullPIDSet AutoWaypointSet;
	FFullPIDSet JoyStickSet;
	VelocitySet.XPID = new QuadPIDController();
	VelocitySet.XPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.XPID->SetGains(-0.03f, 0.f, 0.0f);

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

	VelocitySet.YawPID = new QuadPIDController();
	VelocitySet.YawPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	VelocitySet.YawPID->SetGains(1.0f, 0.0f, 0.0f);

	AutoWaypointSet.XPID = new QuadPIDController();
	AutoWaypointSet.XPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.XPID->SetGains(-0.03f, 0.f, 0.0f);

	AutoWaypointSet.YPID = new QuadPIDController();
	AutoWaypointSet.YPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.YPID->SetGains(0.03f, 0.0f, 0.0f);

	AutoWaypointSet.ZPID = new QuadPIDController();
	AutoWaypointSet.ZPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.ZPID->SetGains(5.f, 0.0f, 0.0f);

	AutoWaypointSet.RollPID = new QuadPIDController();
	AutoWaypointSet.RollPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.RollPID->SetGains(0.31f, 0.2f, 0.34f);

	AutoWaypointSet.PitchPID = new QuadPIDController();
	AutoWaypointSet.PitchPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.PitchPID->SetGains(0.35f, 0.16f, 0.25f);

	AutoWaypointSet.YawPID = new QuadPIDController();
	AutoWaypointSet.YawPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	AutoWaypointSet.YawPID->SetGains(1.0f, 0.0f, 0.0f);

	JoyStickSet.XPID = new QuadPIDController();
	JoyStickSet.XPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.XPID->SetGains(2.329f, 3.626f, 1.832f);

	JoyStickSet.YPID = new QuadPIDController();
	JoyStickSet.YPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.YPID->SetGains(2.329f, 3.626f, 1.832f);

	JoyStickSet.ZPID = new QuadPIDController();
	JoyStickSet.ZPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.ZPID->SetGains(5.344f, 1.f, 0.1f);

	JoyStickSet.RollPID = new QuadPIDController();
	JoyStickSet.RollPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.RollPID->SetGains(11.755f, 5.267f, 9.008f);

	JoyStickSet.PitchPID = new QuadPIDController();
	JoyStickSet.PitchPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.PitchPID->SetGains(11.755f, 5.267f, 9.008f);

	JoyStickSet.YawPID = new QuadPIDController();
	JoyStickSet.YawPID->SetLimits(-maxPIDOutput, maxPIDOutput);
	JoyStickSet.YawPID->SetGains(0.f, 0.f, 0.f);

	PIDMap.Add(EFlightMode::AutoWaypoint, MoveTemp(AutoWaypointSet));
	PIDMap.Add(EFlightMode::VelocityControl, MoveTemp(VelocitySet));
	PIDMap.Add(EFlightMode::JoyStickControl, MoveTemp(JoyStickSet));
	

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
		VelocityControl(a_deltaTime);
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
    const FRotator  worldRot  = dronePawn->GetActorRotation();   // deg

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
    yOut = FMath::Clamp(yOut, -maxAngle, maxAngle);
    xOut = FMath::Clamp(xOut, -maxAngle, maxAngle);

    //  Attitude PIDs  (output: torque commands)
    const double rollError  = yOut - worldRot.Roll;   // deg
    const double pitchError = xOut - worldRot.Pitch;  // deg

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
    FFullPIDSet* CurrentSet = PIDMap.Find(EFlightMode::AutoWaypoint);
    if (!CurrentSet || !dronePawn) return;

    const FVector  currentPos      = dronePawn->GetActorLocation();
    const FVector  worldVel        = dronePawn->GetVelocity();
    const FRotator currentRot      = dronePawn->GetActorRotation();
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

            const float rawRate = CurrentSet->YawPID->Calculate(yawErrDeg, DeltaTime);
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

// ---------------------- Thrust Functions ------------------------

void UQuadDroneController::ThrustMixer(double currentRoll, double currentPitch, double zOutput, double rollOutput, double pitchOutput)
{
	float droneMass = dronePawn->DroneBody->GetMass();
	const float gravity = 980.0f;
	const float hoverThrust = (droneMass * gravity) / 4.0f; 
 
	float baseThrust = hoverThrust + zOutput / 4.0f;
	baseThrust /= FMath::Cos(FMath::DegreesToRadians(FMath::Sqrt(FMath::Pow(currentRoll, 2) + FMath::Pow(currentPitch, 2))));
 
	Thrusts[0] = baseThrust + rollOutput + pitchOutput;
	Thrusts[1] = baseThrust - rollOutput + pitchOutput;
	Thrusts[2] = baseThrust + rollOutput - pitchOutput;
	Thrusts[3] = baseThrust - rollOutput - pitchOutput;
 
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

void UQuadDroneController::YawStabilization(double DeltaTime)
{
	// Early exit if the drone pawn or its physics body is missing.
	if (!dronePawn || !dronePawn->DroneBody) return;

	// Get the drone's forward vector and flatten it to the XY plane (ignore Z).
	FVector CurrentForwardVector = dronePawn->GetActorForwardVector();
	CurrentForwardVector.Z = 0.0f;  // Remove Z component to ensure it's 2D.
	CurrentForwardVector.Normalize();  // Normalize to get unit vector.

	// Normalize the desired forward vector (assumed to be set elsewhere).
	FVector DesiredForwardVector = desiredForwardVector.GetSafeNormal();

	// Compute the angle between the current and desired forward vectors using the dot product.
	float DotProduct = FVector::DotProduct(CurrentForwardVector, DesiredForwardVector);
	FVector CrossProduct = FVector::CrossProduct(CurrentForwardVector, DesiredForwardVector);  // For direction sense.

	// Convert angle from radians to degrees and clamp to avoid precision issues.
	float VectorError = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));
	VectorError = FMath::RadiansToDegrees(VectorError);

	// Determine the sign (clockwise or counter-clockwise) using the cross product and up vector.
	FVector UpVector = dronePawn->DroneBody->GetUpVector();
	float DirectionSign = FMath::Sign(FVector::DotProduct(CrossProduct, UpVector));
	VectorError *= DirectionSign;  // Apply the sign to the angle error.

	// Skip small errors within threshold � prevents overcorrection when close to target.
	static constexpr float YAW_ERROR_THRESHOLD = 1.0f;
	if (FMath::Abs(VectorError) < YAW_ERROR_THRESHOLD)
	{
		return;
	}

	// Retrieve the current PID controller settings for the VelocityControl flight mode.
	FFullPIDSet* CurrentSet = GetPIDSet(currentFlightMode);
	if (!CurrentSet) return;

	// Get the drone's current angular velocity around the Z-axis (current yaw rate).
	FVector CurrentAngularVelocity = dronePawn->DroneBody->GetPhysicsAngularVelocityInDegrees();
	float CurrentYawRate = CurrentAngularVelocity.Z;

	// Compute the desired yaw torque using the PID controller.
	float PIDOutput = CurrentSet->YawPID->Calculate(VectorError, DeltaTime);

	// Clamp the output torque to prevent excessive forces.
	float MaxYawTorque = 2.0f;
	PIDOutput = FMath::Clamp(PIDOutput, -MaxYawTorque, MaxYawTorque);

	// Apply a simple damping term to counteract excess rotational velocity.
	float YawDamping = -CurrentYawRate * 0.05f;
	float FinalYawTorque = PIDOutput + YawDamping;

	// Convert scalar torque into a world-space torque vector.
	FVector TorqueVector = UpVector * FinalYawTorque * YawTorqueForce;

	// Save the applied yaw torque for potential debugging or telemetry.
	LastYawTorqueApplied = FinalYawTorque * YawTorqueForce;

	// Apply the computed torque to each thruster that supports torque input.
	for (UThrusterComponent* Thruster : dronePawn->Thrusters)
	{
		if (Thruster)
		{
			Thruster->ApplyTorque(TorqueVector, true);
		}
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

	float yawTorqueFeedback = CurrentSet->YawPID->Calculate(yawRateError, DeltaTime);

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

// ---------------------- Reset Functions ------------------------

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
		ThisSet.YawPID->Reset();
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
	CurrentSet->YawPID->ResetIntegral();
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