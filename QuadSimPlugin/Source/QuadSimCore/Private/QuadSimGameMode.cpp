#include "QuadSimGameModePlugin.h"
#include "Pawns/QuadPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "SimulationCore/Public/Core/SimulationManager.h"
#include "Core/DroneManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "QuadSimPlayerController.h"
#include "PhysicsEngine/PhysicsSettings.h"

// The AQuadSimGameMode constructor remains the same.
AQuadSimGameMode::AQuadSimGameMode()
{
    PlayerControllerClass = AQuadSimPlayerController::StaticClass();
}

void AQuadSimGameMode::BeginPlay()
{
    Super::BeginPlay();
	if (GEngine)
	{
		// Remove frame rate limits
		GEngine->bUseFixedFrameRate = false;
		GEngine->bSmoothFrameRate = false;
        
		// Set console variables for unlimited FPS
		static const auto CVarMaxFPS = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
		if (CVarMaxFPS)
		{
			CVarMaxFPS->Set(0); // 0 = unlimited
		}
        
		// Disable VSync
		static const auto CVarVSync = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
		if (CVarVSync)
		{
			CVarVSync->Set(0);
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Frame rate limits removed for PX4 compatibility"));
	}
    
	// Configure physics for consistent simulation
	if (UPhysicsSettings* PhysicsSettings = UPhysicsSettings::Get())
	{
		// Enable substepping for more consistent physics
		PhysicsSettings->bSubstepping = true;
		PhysicsSettings->MaxSubstepDeltaTime = 0.01667f; // 60Hz substeps
		PhysicsSettings->MaxSubsteps = 6;
        
		UE_LOG(LogTemp, Warning, TEXT("Physics substepping enabled"));
	}
    if (UWorld* World = GetWorld())
    {
        // Default spawn location in case no PlayerStart is found
        FVector SpawnLocation = FVector::ZeroVector;
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // Find the PlayerStart Actor in the world
        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass());

        if (PlayerStart)
        {
            // If found, use its location and rotation for spawning
            SpawnLocation = PlayerStart->GetActorLocation();
            SpawnRotation = PlayerStart->GetActorRotation();
            UE_LOG(LogTemp, Warning, TEXT("Found PlayerStart at Location: %s"), *SpawnLocation.ToString());
        }
        else
        {
            // Fallback warning if you forget to place a PlayerStart in your level
            UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found in the level! Spawning at world origin."));
        }

        // Spawn SimulationManager at the PlayerStart's location
        ASimulationManager* SimManager = World->SpawnActor<ASimulationManager>(
            ASimulationManager::StaticClass(),
            SpawnLocation,
            SpawnRotation
        );
        
        // Initialize DroneManager pointer
        ADroneManager* DroneManager = nullptr;
        UClass* DroneManagerBP = nullptr;

        // Method 1: Standard plugin content path
        DroneManagerBP = LoadClass<ADroneManager>(nullptr,
            TEXT("/QuadSimPlugin/Blueprints/Core/BP_DroneManager_.BP_DroneManager__C"));

        // Method 2: If that doesn't work, try with Script prefix
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
            // Also spawn DroneManager at the same PlayerStart location
            DroneManager = World->SpawnActor<ADroneManager>(
                DroneManagerBP,
                SpawnLocation,
                SpawnRotation
            );
            UE_LOG(LogTemp, Warning, TEXT("Spawned BP_DroneManager_ at PlayerStart location."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Could not find BP_DroneManager_ blueprint!"));
        }

        // After spawning everything, tell the player's camera where to look.
        if (DroneManager)
        {
            // Get the first player controller
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
            if (PlayerController)
            {
                // Set the camera's view to the DroneManager (or a pawn it contains).
                // This moves the player's view from (0,0,0) to the drone.
                PlayerController->SetViewTargetWithBlend(DroneManager);
                UE_LOG(LogTemp, Warning, TEXT("Set PlayerController view target to DroneManager."));
            }
        }
    }
}