#include "Sensors/IMUSensor.h"

// EVERYTHING IN CM 

UIMUSensor::UIMUSensor()
{

	PrimaryComponentTick.bCanEverTick = false;
	LastAccelerometer = FVector::ZeroVector;
	LastGyroscope = FVector::ZeroVector;
	PreviousVelocity = FVector::ZeroVector;
}

void UIMUSensor::Initialize()
{
	if (GetAttachParent() && GetAttachParent()->IsSimulatingPhysics())
	{
		AttachedBody = Cast<UPrimitiveComponent>(GetAttachParent());
	}
	else if (GetOwner())
	{
		UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		if (RootPrim && RootPrim->IsSimulatingPhysics())
		{
			AttachedBody = RootPrim;
		}
	}
    
	if (AttachedBody)
	{
		PreviousVelocity = AttachedBody->GetPhysicsLinearVelocity();
		bInitialized = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IMUSensor: No physics body found!"));
	}
}

FVector UIMUSensor::SampleRawAcceleration(float DeltaTime)
{
	if (!bInitialized || !AttachedBody || DeltaTime <= 0.f)
		return FVector::ZeroVector;

	FVector CurrVel = AttachedBody->GetPhysicsLinearVelocity();
	FVector RawAccel = (CurrVel - PreviousVelocity) / DeltaTime;
	PreviousVelocity = CurrVel;

	return AttachedBody->GetComponentTransform().InverseTransformVectorNoScale(RawAccel);
}

FVector UIMUSensor::SampleRawAngularVelocity()
{
	if (!bInitialized || !AttachedBody)
		return FVector::ZeroVector;


	FVector RawAngVel = AttachedBody->GetPhysicsAngularVelocityInRadians();

	return AttachedBody->GetComponentTransform().InverseTransformVectorNoScale(RawAngVel);
}

void UIMUSensor::UpdateSensor(float DeltaTime, bool bNoise)
{
	AccumulatedTime += DeltaTime;
	const float Period = 1.f / UpdateRate;

	if (AccumulatedTime < Period)
		return;

	AccumulatedTime -= Period;

	FVector Accel = SampleRawAcceleration(Period);
	FVector Gyro  = SampleRawAngularVelocity();

	if (bNoise)
	{
		Accel.X += SensorNoise() * AccelNoiseStdDev;
		Accel.Y += SensorNoise() * AccelNoiseStdDev;
		Accel.Z += SensorNoise() * AccelNoiseStdDev;

		Gyro.X  += SensorNoise() * GyroNoiseStdDev;
		Gyro.Y  += SensorNoise() * GyroNoiseStdDev;
		Gyro.Z  += SensorNoise() * GyroNoiseStdDev;
	}

	LastAccelerometer = Accel;
	LastGyroscope     = Gyro;
}

float UIMUSensor::SensorNoise()
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
