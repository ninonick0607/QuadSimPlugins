// SimTaskbarWidget.h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimTaskbarWidget.generated.h"

class UButton;
class ADroneManager;
class AQuadPawn;

UCLASS()
class USimTaskbarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;       // keep to refresh manager

	UFUNCTION(BlueprintCallable, Category="SimTaskbar")
	void OnSpawnClicked();

	UFUNCTION(BlueprintCallable, Category="SimTaskbar")
	void RefreshDroneManager();

protected:
	// IMPORTANT: not named BtnSpawn; we will find "BtnSpawn" at runtime
	UPROPERTY(Transient)
	UButton* SpawnButton = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SimTaskbar|Spawn")
	bool bPreferPlayerStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SimTaskbar|Spawn", meta=(ClampMin="0.0"))
	float SpawnSpacing = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SimTaskbar|Spawn")
	bool bPossessAfterSpawn = true;

private:
	TWeakObjectPtr<ADroneManager> DroneManager;

	ADroneManager* GetManager() const;
	FTransform     SuggestSpawnTransform() const;

};
