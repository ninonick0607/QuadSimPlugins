// CoordinateTransform.cpp
#include "CoordinateTransform.h"
#include "GeoReferencingSystem.h"
#include "GeographicCoordinates.h"
#include "EngineUtils.h"
#include "Engine/World.h"

// Define conversion matrices
// Unreal: X=Forward, Y=Right, Z=Up (FRU in cm)
// NED: X=North, Y=East, Z=Down (in meters)
// ENU: X=East, Y=North, Z=Up (in meters)

const FMatrix UCoordinateTransform::UnrealToNEDMat = FMatrix(
    FPlane(0.01f,  0.0f,   0.0f, 0.0f),  // X_NED = X_Unreal / 100
    FPlane(0.0f,  -0.01f,  0.0f, 0.0f),  // Y_NED = -Y_Unreal / 100  
    FPlane(0.0f,   0.0f,  -0.01f, 0.0f), // Z_NED = -Z_Unreal / 100
    FPlane(0.0f,   0.0f,   0.0f, 1.0f)
);

const FMatrix UCoordinateTransform::UnrealToENUMat = FMatrix(
    FPlane(0.0f,  -0.01f,  0.0f, 0.0f),  // X_ENU = -Y_Unreal / 100
    FPlane(0.01f,  0.0f,   0.0f, 0.0f),  // Y_ENU = X_Unreal / 100
    FPlane(0.0f,   0.0f,   0.01f, 0.0f), // Z_ENU = Z_Unreal / 100
    FPlane(0.0f,   0.0f,   0.0f, 1.0f)
);

const FMatrix UCoordinateTransform::NEDToUnrealMat = FMatrix(
    FPlane(100.0f,  0.0f,    0.0f, 0.0f),   // X_Unreal = X_NED * 100
    FPlane(0.0f,   -100.0f,  0.0f, 0.0f),   // Y_Unreal = -Y_NED * 100
    FPlane(0.0f,    0.0f,   -100.0f, 0.0f), // Z_Unreal = -Z_NED * 100
    FPlane(0.0f,    0.0f,    0.0f, 1.0f)
);

const FMatrix UCoordinateTransform::ENUToUnrealMat = FMatrix(
    FPlane(0.0f,   100.0f,  0.0f, 0.0f),   // X_Unreal = Y_ENU * 100
    FPlane(-100.0f, 0.0f,   0.0f, 0.0f),   // Y_Unreal = -X_ENU * 100
    FPlane(0.0f,    0.0f,   100.0f, 0.0f), // Z_Unreal = Z_ENU * 100
    FPlane(0.0f,    0.0f,   0.0f, 1.0f)
);

UCoordinateTransform::UCoordinateTransform()
{
    // Nothing to initialize for static class
}

// ==================== POSITION TRANSFORMS ====================

FVector UCoordinateTransform::UnrealToNED(const FVector& UnrealPos)
{
    // Unreal (cm): X=Forward, Y=Right, Z=Up
    // NED (m): X=North, Y=East, Z=Down
    return FVector(
        UnrealPos.X  ,   // Forward -> North (cm to m)
        -UnrealPos.Y ,  // Right -> East (flip and cm to m)
        -UnrealPos.Z    // Up -> Down (flip and cm to m)
    );
}

FVector UCoordinateTransform::NEDToUnreal(const FVector& NEDPos)
{
    // NED (m): X=North, Y=East, Z=Down
    // Unreal (cm): X=Forward, Y=Right, Z=Up
    return FVector(
        NEDPos.X ,    
        -NEDPos.Y,   
        -NEDPos.Z
    );
}

FVector UCoordinateTransform::UnrealToENU(const FVector& UnrealPos)
{
    // Unreal (cm): X=Forward, Y=Right, Z=Up
    // ENU (m): X=East, Y=North, Z=Up
    return FVector(
        -UnrealPos.Y,  // Right -> East (flip and cm to m)
        UnrealPos.X,   // Forward -> North (cm to m)
        UnrealPos.Z    // Up -> Up (cm to m)
    );
}

FVector UCoordinateTransform::ENUToUnreal(const FVector& ENUPos)
{
    // ENU (m): X=East, Y=North, Z=Up
    // Unreal (cm): X=Forward, Y=Right, Z=Up
    return FVector(
        ENUPos.Y,    // North -> Forward (m to cm)
        -ENUPos.X,   // East -> Right (flip and m to cm)
        ENUPos.Z      // Up -> Up (m to cm)
    );
}

// ==================== VELOCITY TRANSFORMS ====================

FVector UCoordinateTransform::UnrealVelocityToNED(const FVector& UnrealVel)
{
    // Same transformation as position but for velocity (cm/s to m/s)
    return UnrealToNED(UnrealVel);
}

FVector UCoordinateTransform::UnrealVelocityToENU(const FVector& UnrealVel)
{
    // Same transformation as position but for velocity (cm/s to m/s)
    return UnrealToENU(UnrealVel);
}

// ==================== ROTATION TRANSFORMS ====================

FRotator UCoordinateTransform::UnrealRotationToNED(const FRotator& UnrealRot)
{
    // Unreal uses FRotator(Pitch, Yaw, Roll)
    // NED typically uses aerospace convention
    // This is a simplified conversion - may need adjustment based on your specific needs
    
    return FRotator(
        -UnrealRot.Pitch,  // Pitch sign flip
        UnrealRot.Yaw,     // Yaw stays same
        UnrealRot.Roll     // Roll stays same
    );
}

FRotator UCoordinateTransform::UnrealRotationToENU(const FRotator& UnrealRot)
{
    // Convert to ENU frame
    // ENU typically uses different conventions
    
    return FRotator(
        UnrealRot.Pitch,           // Pitch
        UnrealRot.Yaw + 90.0f,     // Yaw offset by 90 degrees
        UnrealRot.Roll             // Roll
    );
}

FQuat UCoordinateTransform::UnrealQuaternionToNED(const FQuat& UnrealQuat)
{
    // For quaternions, we need to consider the coordinate system change
    // This matches your PX4Component conversion
    return FQuat(
        UnrealQuat.W,   // W stays same
        UnrealQuat.X,   // X stays same
        -UnrealQuat.Y,  // Y flips
        -UnrealQuat.Z   // Z flips
    );
}

FQuat UCoordinateTransform::UnrealQuaternionToENU(const FQuat& UnrealQuat)
{
    // Convert quaternion to ENU frame
    // First convert to rotation, transform, then back to quaternion
    FRotator UnrealRot = UnrealQuat.Rotator();
    FRotator ENURot = UnrealRotationToENU(UnrealRot);
    return FQuat(ENURot);
}

// ==================== ANGULAR VELOCITY TRANSFORMS ====================

FVector UCoordinateTransform::UnrealAngularVelocityToNED(const FVector& UnrealAngVel)
{
    // Convert from deg/s to rad/s and apply coordinate transform
    // Unreal: X=Roll, Y=Pitch, Z=Yaw (deg/s)
    // NED: X=Roll, Y=Pitch, Z=Yaw (rad/s)
    
    return FVector(
        FMath::DegreesToRadians(UnrealAngVel.X),      // Roll
        FMath::DegreesToRadians(-UnrealAngVel.Y),     // Pitch (flip)
        FMath::DegreesToRadians(-UnrealAngVel.Z)      // Yaw (flip)
    );
}

FVector UCoordinateTransform::UnrealAngularVelocityToENU(const FVector& UnrealAngVel)
{
    // Convert from deg/s to rad/s and apply coordinate transform
    return FVector(
        FMath::DegreesToRadians(UnrealAngVel.X),      // Roll
        FMath::DegreesToRadians(UnrealAngVel.Y),      // Pitch
        FMath::DegreesToRadians(UnrealAngVel.Z)       // Yaw
    );
}

// ==================== FRAME TRANSFORMS ====================

FVector UCoordinateTransform::WorldToBodyFrame(const FVector& WorldVector, const FRotator& BodyRotation)
{
    // Transform from world frame to body frame
    return BodyRotation.UnrotateVector(WorldVector);
}

FVector UCoordinateTransform::WorldToBodyFrameQuat(const FVector& WorldVector, const FQuat& BodyQuat)
{
    // Transform from world frame to body frame using quaternion
    return BodyQuat.UnrotateVector(WorldVector);
}

FVector UCoordinateTransform::BodyToWorldFrame(const FVector& BodyVector, const FRotator& BodyRotation)
{
    // Transform from body frame to world frame
    return BodyRotation.RotateVector(BodyVector);
}

// ==================== BATCH OPERATIONS ====================

void UCoordinateTransform::BatchUnrealToNED(const TArray<FVector>& UnrealPositions, TArray<FVector>& OutNEDPositions)
{
    OutNEDPositions.Reset(UnrealPositions.Num());
    
    // Use matrix multiplication for efficiency
    for (const FVector& Pos : UnrealPositions)
    {
        OutNEDPositions.Add(UnrealToNED(Pos));
    }
}

void UCoordinateTransform::BatchUnrealToENU(const TArray<FVector>& UnrealPositions, TArray<FVector>& OutENUPositions)
{
    OutENUPositions.Reset(UnrealPositions.Num());
    
    for (const FVector& Pos : UnrealPositions)
    {
        OutENUPositions.Add(UnrealToENU(Pos));
    }
}

// ==================== GEOGRAPHIC CONVERSIONS ====================

FVector UCoordinateTransform::GeographicToUnreal(float Latitude, float Longitude, float Altitude, 
                                                  UObject* WorldContextObject, AGeoReferencingSystem* GeoRef)
{
    // Find GeoReferencingSystem if not provided
    if (!GeoRef && WorldContextObject)
    {
        UWorld* World = WorldContextObject->GetWorld();
        if (World)
        {
            for (TActorIterator<AGeoReferencingSystem> It(World); It; ++It)
            {
                GeoRef = *It;
                break;
            }
        }
    }
    
    if (!GeoRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("CoordinateTransform: No GeoReferencingSystem found!"));
        return FVector::ZeroVector;
    }
    
    // Create geographic coordinates
    FGeographicCoordinates GeoCoords;
    GeoCoords.Latitude = Latitude;
    GeoCoords.Longitude = Longitude;
    GeoCoords.Altitude = Altitude;
    
    // Convert to Unreal coordinates
    FVector UnrealPos;
    GeoRef->GeographicToEngine(GeoCoords, UnrealPos);
    
    return UnrealPos;
}

void UCoordinateTransform::UnrealToGeographic(const FVector& UnrealPos, float& OutLatitude, float& OutLongitude, 
                                               float& OutAltitude, UObject* WorldContextObject, AGeoReferencingSystem* GeoRef)
{
    // Find GeoReferencingSystem if not provided
    if (!GeoRef && WorldContextObject)
    {
        UWorld* World = WorldContextObject->GetWorld();
        if (World)
        {
            for (TActorIterator<AGeoReferencingSystem> It(World); It; ++It)
            {
                GeoRef = *It;
                break;
            }
        }
    }
    
    if (!GeoRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("CoordinateTransform: No GeoReferencingSystem found!"));
        OutLatitude = 0.0f;
        OutLongitude = 0.0f;
        OutAltitude = 0.0f;
        return;
    }
    
    // Convert to geographic coordinates
    FGeographicCoordinates GeoCoords;
    GeoRef->EngineToGeographic(UnrealPos, GeoCoords);
    
    OutLatitude = GeoCoords.Latitude;
    OutLongitude = GeoCoords.Longitude;
    OutAltitude = GeoCoords.Altitude;
}

// ==================== UTILITY FUNCTIONS ====================

FMatrix UCoordinateTransform::GetUnrealToNEDMatrix()
{
    return UnrealToNEDMat;
}

FMatrix UCoordinateTransform::GetUnrealToENUMatrix()
{
    return UnrealToENUMat;
}

FQuat UCoordinateTransform::NormalizeQuaternion(const FQuat& Quat)
{
    FQuat NormalizedQuat = Quat;
    NormalizedQuat.Normalize();
    return NormalizedQuat;
}

FRotator UCoordinateTransform::ValidateEulerAngles(const FRotator& Euler)
{
    // Clamp angles to valid ranges
    FRotator ValidatedEuler;
    ValidatedEuler.Pitch = FMath::ClampAngle(Euler.Pitch, -90.0f, 90.0f);
    ValidatedEuler.Yaw = FMath::ClampAngle(Euler.Yaw, -180.0f, 180.0f);
    ValidatedEuler.Roll = FMath::ClampAngle(Euler.Roll, -180.0f, 180.0f);
    return ValidatedEuler;
}