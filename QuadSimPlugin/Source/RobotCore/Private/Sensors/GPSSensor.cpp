#include "Sensors/GPSSensor.h"

// EVERYTHING IN CM 

UGPSSensor::UGPSSensor()
{

	PrimaryComponentTick.bCanEverTick = false;
	LastGPS = FVector::ZeroVector;
}

FVector UGPSSensor::SampleRawGPS() const
{
	// GPS sensors measure the position of the vehicle, not the sensor itself
	// So we should use the owner's (drone's) location
	if (AActor* Owner = GetOwner())
	{
		return Owner->GetActorLocation();
	}
	
	// Fallback to component location if no owner
	return GetComponentLocation();
}

void UGPSSensor::UpdateSensor(float DeltaTime, bool bNoise)
{
	AccumulatedTime += DeltaTime;
	const float Period = 1.f / UpdateRate;

	if (AccumulatedTime < Period)
		return;
	AccumulatedTime -= Period;

	// Log update rate verification
	static float LastLogTime = 0.0f;
	static int32 UpdateCount = 0;
	UpdateCount++;
	
	float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if (CurrentTime - LastLogTime >= 1.0f) // Log once per second
	{
		UE_LOG(LogTemp, Display, TEXT("GPS Update Rate: %d Hz (Expected: %.0f Hz)"), UpdateCount, UpdateRate);
		UpdateCount = 0;
		LastLogTime = CurrentTime;
	}

	FVector Pos = SampleRawGPS();

	if (bNoise)
	{
		Pos.X += SensorNoise() * LatLonNoiseStdDev;
		Pos.Y += SensorNoise() * LatLonNoiseStdDev;
		Pos.Z += SensorNoise() * AltNoiseStdDev;
	}

	LastGPS = Pos;
}

float UGPSSensor::SensorNoise()
{
	// Box–Muller
	static bool bPhase = true;
	static float V1, V2, S;
	float X;

	if (bPhase)
	{
		do {
			float U1 = FMath::FRand();
			float U2 = FMath::FRand();
			V1 = 2.f * U1 - 1.f;
			V2 = 2.f * U2 - 1.f;
			S  = V1 * V1 + V2 * V2;
		} while (S >= 1.f || S < KINDA_SMALL_NUMBER);

		X = V1 * FMath::Sqrt(-2.f * FMath::Loge(S) / S);
	}
	else
	{
		X = V2 * FMath::Sqrt(-2.f * FMath::Loge(S) / S);
	}

	bPhase = !bPhase;
	return X;
}