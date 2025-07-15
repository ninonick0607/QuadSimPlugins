#include "QuadSimGameModePlugin.h"
#include "Pawns/QuadPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "SimulationCore/Public/Core/SimulationManager.h"  
#include "Core/DroneManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"  

AQuadSimGameMode::AQuadSimGameMode()
{
}

void AQuadSimGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        // Spawn SimulationManager (C++ class)
        ASimulationManager* SimManager = World->SpawnActor<ASimulationManager>(
            ASimulationManager::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator
        );

        // Try multiple path formats for plugin content
        UClass* DroneManagerBP = nullptr;

        // Method 1: Standard plugin content path
        DroneManagerBP = LoadClass<ADroneManager>(nullptr,
            TEXT("/QuadSimPlugin/Blueprints/Core/BP_DroneManager_.BP_DroneManager__C"));

        // Method 2: If that doesn't work, try with Script prefix (remove spaces!)
        if (!DroneManagerBP)
        {
            DroneManagerBP = LoadClass<ADroneManager>(nullptr,
                TEXT("/Script/Engine.Blueprint'/QuadSimPlugin/Blueprints/Core/BP_DroneManager_.BP_DroneManager__C'"));
        }

        // Method 3: Alternative plugin path
        if (!DroneManagerBP)
        {
            DroneManagerBP = LoadClass<ADroneManager>(nullptr,
                TEXT("Blueprint'/QuadSimPlugin/Blueprints/Core/BP_DroneManager_.BP_DroneManager__C'"));
        }

        if (DroneManagerBP)
        {
            ADroneManager* DroneManager = World->SpawnActor<ADroneManager>(
                DroneManagerBP,
                FVector::ZeroVector,
                FRotator::ZeroRotator
            );
            UE_LOG(LogTemp, Warning, TEXT("Spawned BP_DroneManager_"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Could not find BP_DroneManager_ blueprint!"));

            // Alternative: Use ConstructorHelpers (but this only works in constructor)
            // So let's find it differently
            TArray<UObject*> FoundObjects;
            EngineUtils::FindOrLoadAssetsByPath(TEXT("/QuadSimPlugin/Blueprints/Core/"), FoundObjects, EngineUtils::ATL_Class);

            for (UObject* Obj : FoundObjects)
            {
                UE_LOG(LogTemp, Warning, TEXT("Found object: %s"), *Obj->GetName());
            }
        }
    }
}