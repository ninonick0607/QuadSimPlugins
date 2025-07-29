// IMUSensor.h
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "IMUSensor.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTCORE_API UIMUSensor : public USceneComponent
{
	GENERATED_BODY()

public: 
	UIMUSensor();
    
	float SensorNoise();
	void UpdateSensor(float DeltaTime, bool bNoise);
	FVector SampleRawAcceleration(float DeltaTime);
	FVector SampleRawAngularVelocity();
	FVector SampleRawVelocity();
	FRotator SampleRawAttitude();
	
	FVector GetLastAccelerometer() { return LastAccelerometer; }
	FVector GetLastGyroscope() { return LastGyroscope; }
	FVector GetLastGyroscopeDegrees() { return  FMath::RadiansToDegrees(LastGyroscope); }
	FVector GetLastVelocity(){return LastVelocity;}
	FRotator GetLastAttitude(){return LastAttitude;}
	void Initialize();

private:
	float UpdateRate = 250.0f;
	float AccumulatedTime = 0.0f;
	FVector LastAccelerometer;
	FVector LastGyroscope;
    FVector LastVelocity;
	FRotator LastAttitude;
	FVector PreviousVelocity;
	bool bInitialized = false;
    
	// Noise parameters
	float AccelVelNoiseStdDev = 0.02f; // m/s^2
	float GyroAttNoiseStdDev = 0.01f;  // rad/s
    
	UPROPERTY()
	UPrimitiveComponent* AttachedBody = nullptr;
};