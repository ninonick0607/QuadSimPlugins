// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TextureRenderTarget2D.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CameraHUD.generated.h"

/**
 * 
 */
UCLASS()
class QUADSIMCORE_API ACameraHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

private:
	/** Cache pointer after first Draw */
	UTextureRenderTarget2D* CachedFPVRT = nullptr;
};
