#include "Sensors/GPSSensor.h"


float UGPSSensor::SensorNoise()
{
	static float V1, V2, S;
	static bool bPhase = true;
	float X;

	if (bPhase) 
	{
		do {
			float U1 = FMath::FRand(); 
			float U2 = FMath::FRand();
			V1 = 2.0f * U1 - 1.0f;
			V2 = 2.0f * U2 - 1.0f;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1.0f || FMath::Abs(S) < 1e-8f);

		X = V1 * FMath::Sqrt(-2.0f * FMath::Loge(S) / S);
	} 
	else 
	{
		X = V2 * FMath::Sqrt(-2.0f * FMath::Loge(S) / S);
	}

	bPhase = !bPhase;
	return X;
}

UGPSSensor::UGPSSensor()
{

	PrimaryComponentTick.bCanEverTick = true;
	LastGPS = FVector::ZeroVector;
}

FVector UGPSSensor::ReadCurrentGPS()
{
	FVector CurrentGPS = GetComponentLocation();
	return CurrentGPS;
}

void UGPSSensor::UpdateSensor(float DeltaTime, bool bNoise )
{
	AccumalatedTime += DeltaTime;
	float UpdatePeriod = 1.0f / UpdateRate;

	if (AccumalatedTime >= UpdatePeriod)
	{
		AccumalatedTime -= UpdatePeriod;
		LastGPS = ReadCurrentGPS();
		
		float LatLonNoiseMeters = 5.f; 
		float AltNoiseMeters = 10.f;

		if (bNoise)
		{
			LastGPS.X += SensorNoise()*LatLonNoiseMeters;
			LastGPS.Y += SensorNoise()*LatLonNoiseMeters;
			LastGPS.Z += SensorNoise()*AltNoiseMeters;
		}
	}
}

