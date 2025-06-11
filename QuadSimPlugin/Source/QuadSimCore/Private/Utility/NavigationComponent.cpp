#include "Utility/NavigationComponent.h"
#include "Core/DroneJSONConfig.h"

UNavigationComponent::UNavigationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentIndex = 0;
}
// Called when the game starts
void UNavigationComponent::BeginPlay()
{
    Super::BeginPlay();
    // Initialize AcceptableDistance from JSON config
    AcceptableDistance = UDroneJSONConfig::Get().Config.FlightParams.AcceptableDistance;
}

void UNavigationComponent::SetNavigationPlan(const TArray<FVector>& InWaypoints)
{
    Waypoints = InWaypoints;
    CurrentIndex = 0;
}

FVector UNavigationComponent::GetCurrentSetpoint() const
{
    if (Waypoints.IsValidIndex(CurrentIndex))
    {
        return Waypoints[CurrentIndex];
    }
    return FVector::ZeroVector;
}

void UNavigationComponent::UpdateNavigation(const FVector& CurrentPosition)
{
    if (!Waypoints.IsValidIndex(CurrentIndex))
        return;

    if (FVector::Dist(CurrentPosition, Waypoints[CurrentIndex]) <= AcceptableDistance)
    {
        CurrentIndex++;
        if (CurrentIndex >= Waypoints.Num())
        {
            CurrentIndex = 0; // Or handle plan completion as needed.
        }
    }
}

void UNavigationComponent::ResetNavigation()
{
    CurrentIndex = 0;
}

void UNavigationComponent::SetCurrentDestination(const FVector& Destination)
{
    Waypoints.Empty();
    Waypoints.Add(Destination);
    CurrentIndex = 0;
}

void UNavigationComponent::AddWaypoint(const FVector& Waypoint)
{
    Waypoints.Add(Waypoint);
}
