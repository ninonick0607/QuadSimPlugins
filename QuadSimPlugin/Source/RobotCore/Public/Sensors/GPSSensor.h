// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GPSSensor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTCORE_API UGPSSensor : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGPSSensor();
	float SensorNoise();
	void UpdateSensor(float DeltaTime, bool bNoise );
	FVector ReadCurrentGPS();
	
	FVector GetLastGPS(){return LastGPS;}
	
private:
	float UpdateRate = 250.0f;
	float AccumalatedTime = 0.0f;
	FVector LastGPS;
	
};
