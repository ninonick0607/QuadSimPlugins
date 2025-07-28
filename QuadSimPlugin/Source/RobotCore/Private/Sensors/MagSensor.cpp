// Fill out your copyright notice in the Description page of Project Settings.

#include "Sensors/MagSensor.h"
#include "Sensors/GPSSensor.h"
#include "Sensors/SensorManagerComponent.h"
#include "WorldMagneticModel/GeoMagDeclination.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Math/UnrealMathUtility.h"
#include "EngineUtils.h" 
#include "GeoReferencingSystem.h"
#include "GeographicCoordinates.h"
UMagSensor::UMagSensor()
{
    PrimaryComponentTick.bCanEverTick = false;
    LastMagField = FVector::ZeroVector;
    EarthMagField = FVector::ZeroVector;
}

void UMagSensor::Initialize()
{
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AGeoReferencingSystem> It(World); It; ++It)
		{
			GeoRefSystem = *It;
			break;
		}
        
		if (!GeoRefSystem)
		{
			UE_LOG(LogTemp, Warning, TEXT("MagSensor: No GeoReferencingSystem found in level! Please add one."));
		}
	}
    // Any initialization if needed
    bEarthMagFieldValid = false;
    UE_LOG(LogTemp, Display, TEXT("MagSensor: Initialized"));
}

void UMagSensor::UpdateSensor(float DeltaTime, bool bNoise)
{
    AccumulatedTime += DeltaTime;
    const float Period = 1.0f / UpdateRate;
    
    if (AccumulatedTime < Period)
    {
        return;
    }
    AccumulatedTime -= Period;
    
    // First, update the Earth's magnetic field based on GPS position
    UpdateEarthMagField();
    
    if (!bEarthMagFieldValid)
    {
        // No valid magnetic field data yet
        return;
    }
    
    // Transform Earth field to body frame
    FVector BodyMagField = TransformToBodyFrame(EarthMagField);
	static float BodyLogTimer = 0.0f;
	BodyLogTimer += Period;
	if (BodyLogTimer >= 2.0f)
	{
		FRotator DroneRot = GetOwner()->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Body Frame Field (PX4 output): X=%.3f, Y=%.3f, Z=%.3f Gauss"), 
			   BodyMagField.X, BodyMagField.Y, BodyMagField.Z);
		UE_LOG(LogTemp, Warning, TEXT("Drone Rotation: Roll=%.1f°, Pitch=%.1f°, Yaw=%.1f°"), 
			   DroneRot.Roll, DroneRot.Pitch, DroneRot.Yaw);
		UE_LOG(LogTemp, Warning, TEXT("================================"));
		BodyLogTimer = 0.0f;
	}
    // Add noise if enabled
    if (bNoise)
    {
        BodyMagField += NoiseGauss3f(MagNoiseStdDev, MagNoiseStdDev, MagNoiseStdDev * 1.5f);
    }
    
    // Add sensor bias/offset
    BodyMagField.X += MagOffsetX;
    BodyMagField.Y += MagOffsetY;
    BodyMagField.Z += MagOffsetZ;
    
    // Store the result
    LastMagField = BodyMagField;
}

void UMagSensor::UpdateEarthMagField()
{
	// Check if we have georeferencing system
	if (!GeoRefSystem)
	{
		return;
	}
    
	// Get the drone's world position
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
    
	// Get current position in Unreal coordinates
	FVector WorldPosition = Owner->GetActorLocation();
    
	// Convert to geographic coordinates using the Georeferencing plugin
	FGeographicCoordinates GeoCoords;
	GeoRefSystem->EngineToGeographic(WorldPosition, GeoCoords);
    
	// Get the actual latitude and longitude
	float Latitude = GeoCoords.Latitude;
	float Longitude = GeoCoords.Longitude;
	float Altitude = GeoCoords.Altitude;
    
	// Get magnetic field vector from World Magnetic Model
	FVector MagFieldNED = FGeoMagDeclination::GetMagFieldVector(Latitude, Longitude);
    
	// The magnetic field is already in NED (North-East-Down) frame
	// We need to convert it to Unreal Engine coordinates
	// Get the ENU vectors at this location
	FVector EastVector, NorthVector, UpVector;
	GeoRefSystem->GetENUVectorsAtGeographicLocation(GeoCoords, EastVector, NorthVector, UpVector);
    
	// Transform magnetic field from NED to Unreal coordinates
	// NED: North, East, Down
	// Transform to world coordinates using the local tangent frame
	EarthMagField = MagFieldNED.X * NorthVector + 
				   MagFieldNED.Y * EastVector + 
				   MagFieldNED.Z * (-UpVector); // Down = -Up
    
	bEarthMagFieldValid = true;
	float Declination = FGeoMagDeclination::GetMagDeclinationDegrees(Latitude, Longitude);
	float Inclination = FGeoMagDeclination::GetMagInclinationDegrees(Latitude, Longitude);
	float Strength = FGeoMagDeclination::GetMagStrengthGauss(Latitude, Longitude);
    	
	static float LogTimer = 0.0f;
	LogTimer += 0.01f; // Assuming 100Hz update rate
	if (LogTimer >= 2.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== MAGNETOMETER VERIFICATION ==="));
		UE_LOG(LogTemp, Warning, TEXT("Location: Lat=%.6f°, Lon=%.6f°, Alt=%.1fm"), 
			   Latitude, Longitude, Altitude);
		UE_LOG(LogTemp, Warning, TEXT("Magnetic Components:"));
		UE_LOG(LogTemp, Warning, TEXT("  - Declination: %.2f° (angle from true north)"), Declination);
		UE_LOG(LogTemp, Warning, TEXT("  - Inclination: %.2f° (dip angle)"), Inclination);
		UE_LOG(LogTemp, Warning, TEXT("  - Total Strength: %.3f Gauss (%.1f μT)"), 
			   Strength, Strength * 100.0f); // 1 Gauss = 100 μT
		UE_LOG(LogTemp, Warning, TEXT("NED Frame Field: North=%.3f, East=%.3f, Down=%.3f Gauss"), 
			   MagFieldNED.X, MagFieldNED.Y, MagFieldNED.Z);
		UE_LOG(LogTemp, Warning, TEXT("Earth Frame Field: X=%.3f, Y=%.3f, Z=%.3f Gauss"), 
			   EarthMagField.X, EarthMagField.Y, EarthMagField.Z);
		LogTimer = 0.0f;
	}
}

FVector UMagSensor::TransformToBodyFrame(const FVector& EarthField)
{
    // Get the vehicle's rotation
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return EarthField;
    }
    
    // Get the inverse rotation to transform from world to body frame
    FQuat VehicleRotation = Owner->GetActorQuat();
    FQuat InverseRotation = VehicleRotation.Inverse();
    
    // Transform the magnetic field vector from world frame to body frame
    return InverseRotation.RotateVector(EarthField);
}

FVector UMagSensor::NoiseGauss3f(float StdX, float StdY, float StdZ)
{
    return FVector(
        SensorNoise() * StdX,
        SensorNoise() * StdY,
        SensorNoise() * StdZ
    );
}

float UMagSensor::SensorNoise()
{
    // Box-Muller transform for Gaussian noise (same as other sensors)
    static bool bPhase = true;
    static float V1, V2, S;
    float X;
    
    if (bPhase)
    {
        do {
            float U1 = FMath::FRand();
            float U2 = FMath::FRand();
            V1 = 2.0f * U1 - 1.0f;
            V2 = 2.0f * U2 - 1.0f;
            S = V1 * V1 + V2 * V2;
        } while (S >= 1.0f || S < KINDA_SMALL_NUMBER);
        
        X = V1 * FMath::Sqrt(-2.0f * FMath::Loge(S) / S);
    }
    else
    {
        X = V2 * FMath::Sqrt(-2.0f * FMath::Loge(S) / S);
    }
    
    bPhase = !bPhase;
    return X;
}