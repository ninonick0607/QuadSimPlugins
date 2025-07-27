// SensorManagerComponent.cpp
#include "Sensors/SensorManagerComponent.h"
#include "Sensors/GPSSensor.h"
#include "Sensors/IMUSensor.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

USensorManagerComponent::USensorManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false; 
    bSensorsInitialized = false;
    
    GPS = CreateDefaultSubobject<UGPSSensor>(TEXT("GPSSensor"));
    IMU = CreateDefaultSubobject<UIMUSensor>(TEXT("IMUSensor"));
}

void USensorManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GPS && !GPS->GetAttachParent())
	{
		GPS->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		UE_LOG(LogTemp, Display, TEXT("SensorManager: Attached GPS to SensorManager"));
	}
	
	if (IMU && !IMU->GetAttachParent())
	{
		IMU->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		UE_LOG(LogTemp, Display, TEXT("SensorManager: Attached IMU to SensorManager"));
	}
}

void USensorManagerComponent::InitializeSensors()
{
    if (bSensorsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("SensorManager: Sensors already initialized"));
        return;
    }
    
    if (IMU)
    {
        IMU->Initialize();
        UE_LOG(LogTemp, Display, TEXT("SensorManager: IMU initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SensorManager: IMU component is null!"));
    }
    
    if (GPS)
    {
        UE_LOG(LogTemp, Display, TEXT("SensorManager: GPS ready"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SensorManager: GPS component is null!"));
    }
	
    bSensorsInitialized = true;
    UE_LOG(LogTemp, Display, TEXT("SensorManager: All sensors initialized"));
}

void USensorManagerComponent::UpdateAllSensors(float DeltaTime, bool bAddNoise)
{
    if (!bSensorsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("SensorManager: Attempting to update uninitialized sensors"));
        return;
    }
    
    // GPS updates normally (10Hz)
    if (GPS)
    {
        GPS->UpdateSensor(DeltaTime, bAddNoise);
    }
    
    // IMU needs to update at 250Hz, so we may need multiple updates per frame
    if (IMU)
    {
        const float IMUPeriod = 1.0f / 250.0f; // 0.004 seconds (4ms)
        float RemainingTime = DeltaTime;
        
        // Update IMU multiple times if frame time is longer than IMU period
        while (RemainingTime > 0.0f)
        {
            float StepTime = FMath::Min(RemainingTime, IMUPeriod);
            IMU->UpdateSensor(StepTime, bAddNoise);
            RemainingTime -= IMUPeriod;
        }
    }
}

// void USensorManagerComponent::SetupSensorAttachments()
// {
//     // Currently sensors are attached to this component in the constructor
//     // Override this if you need special attachment logic
//     
//     // Example: If you wanted IMU attached to a specific socket:
//     // if (IMU && GetAttachParent())
//     // {
//     //     IMU->AttachToComponent(GetAttachParent(), FAttachmentTransformRules::KeepRelativeTransform, "IMUSocket");
//     // }
// }