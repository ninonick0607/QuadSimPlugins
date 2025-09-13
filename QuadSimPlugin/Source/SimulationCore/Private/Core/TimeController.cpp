#include "SimulationCore/Public/Core/TimeController.h"
#include "Misc/OutputDeviceNull.h"

UTimeController::UTimeController()
{
	FixedTimestep = 0.004f;      // 250 Hz default for robotics
	Accumulator   = 0.0f;
	SimSpeed      = 1.0f;        // 1x real-time accumulation
	bPaused       = false;
	MaxAccumulatedTime = 0.05f;  // cap ~50 ms backlog (12–13 steps at 250 Hz)
}

void UTimeController::AccumulateTime(float DeltaWallSeconds)
{
	if (bPaused) return;

	// In Robotics mode we'll call this with DeltaWallSeconds; SimSpeed scales wall->sim.
	const float Scaled = DeltaWallSeconds * SimSpeed;
	Accumulator += Scaled;

	if (Accumulator > MaxAccumulatedTime)
		Accumulator = MaxAccumulatedTime;
}

bool UTimeController::ShouldStep() const
{
	return Accumulator >= FixedTimestep;
}

void UTimeController::ConsumeOneStep()
{
	Accumulator -= FixedTimestep;
	if (Accumulator < 0.0f) Accumulator = 0.0f;
}

void UTimeController::Reset()
{
	Accumulator = 0.0f;
}

void UTimeController::SetFixedTimestep(float NewTimestep)
{
	if (NewTimestep > 0.0f)
	{
		FixedTimestep = NewTimestep;
		UE_LOG(LogTemp, Display, TEXT("Fixed timestep = %.4f s (%.1f Hz)"),
			   FixedTimestep, 1.0f / FixedTimestep);
	}
}

void UTimeController::SetSimSpeed(float NewSimSpeed)
{
	SimSpeed = FMath::Clamp(NewSimSpeed, 0.01f, 100.0f);
	UE_LOG(LogTemp, Display, TEXT("SimSpeed (Robotics mode) = %.2fx"), SimSpeed);
}

void UTimeController::SetPaused(bool bNewPaused)
{
	bPaused = bNewPaused;
	if (bPaused) Accumulator = 0.0f;
}
