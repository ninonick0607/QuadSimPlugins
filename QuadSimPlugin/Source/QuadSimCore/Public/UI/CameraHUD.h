// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/TextureRenderTarget2D.h" // Good practice to include here
#include "CameraHUD.generated.h"

UCLASS()
class QUADSIMCORE_API ACameraHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

private:
	/** Cache pointer after first Draw, now safe for Garbage Collection */
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> CachedFPVRT = nullptr;
};