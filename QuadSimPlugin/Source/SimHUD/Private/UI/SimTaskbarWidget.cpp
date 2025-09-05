#include "UI/SimTaskbarWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

// Cross-module types (from QuadSimCore)
#include "QuadSimCore/Public/Core/DroneManager.h"																																																																						
#include "QuadSimCore/Public/Pawns/QuadPawn.h"																																																																						

void USimTaskbarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!SpawnButton)
	{
		SpawnButton = Cast<UButton>(GetWidgetFromName(TEXT("BtnSpawn")));
		if (!SpawnButton)
		{
			UE_LOG(LogTemp, Error, TEXT("[SimTaskbar] Button 'BtnSpawn' NOT FOUND"));
			return;
		}
	}
	SpawnButton->OnClicked.Clear();
	SpawnButton->OnClicked.AddDynamic(this, &USimTaskbarWidget::OnSpawnClicked);
	UE_LOG(LogTemp, Log, TEXT("[SimTaskbar] Bound OnClicked for BtnSpawn"));

	RefreshDroneManager();
}


void USimTaskbarWidget::RefreshDroneManager()
{
    if (UWorld* World = GetWorld())
    {
        DroneManager = ADroneManager::Get(World);
        if (!DroneManager.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("SimTaskbar: DroneManager not found in world."));
        }
    }
}

ADroneManager* USimTaskbarWidget::GetManager() const
{
    if (DroneManager.IsValid())
    {
        return DroneManager.Get();
    }

    // Fallback re-query
    if (UWorld* World = GetWorld())
    {
        return ADroneManager::Get(World);
    }
    return nullptr;
}

void USimTaskbarWidget::OnSpawnClicked()
{
    ADroneManager* DM = GetManager();
    if (!DM)
    {
        UE_LOG(LogTemp, Error, TEXT("SimTaskbar: No DroneManager available."));
        return;
    }

    const FTransform SpawnXform = SuggestSpawnTransform();

    AQuadPawn* NewDrone = DM->SpawnDrone(SpawnXform.GetLocation(), SpawnXform.Rotator());
    if (!NewDrone)
    {
        UE_LOG(LogTemp, Error, TEXT("SimTaskbar: SpawnDrone failed (check DroneManager.QuadPawnClass)."));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("SimTaskbar: Spawned drone at %s"), *SpawnXform.GetLocation().ToString());

    if (bPossessAfterSpawn)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
        {
            PC->Possess(NewDrone);
        }
    }
}

FTransform USimTaskbarWidget::SuggestSpawnTransform() const
{
    FVector BaseLoc = FVector::ZeroVector;
    FRotator BaseRot = FRotator::ZeroRotator;

    UWorld* World = GetWorld();
    if (!World)
    {
        return FTransform(BaseRot, BaseLoc);
    }

    // 1) Prefer PlayerStart for base location
    if (bPreferPlayerStart)
    {
        if (AActor* PS = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()))
        {
            BaseLoc = PS->GetActorLocation();
            BaseRot = PS->GetActorRotation();
        }
    }

    // 2) If we still don't have a good base, use the player's pawn
    if (BaseLoc.IsNearlyZero())
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        {
            if (APawn* P = PC->GetPawn())
            {
                BaseLoc = P->GetActorLocation();
                BaseRot = P->GetActorRotation();
            }
        }
    }

    // 3) Offset by current drone count to avoid overlap
    int32 Count = 0;
    if (ADroneManager* DM = const_cast<USimTaskbarWidget*>(this)->GetManager())
    {
        Count = DM->GetDroneList().Num();
    }

    const FVector Offset(SpawnSpacing * Count, 0.f, 0.f);
    BaseLoc += Offset;

    return FTransform(BaseRot, BaseLoc);
}
