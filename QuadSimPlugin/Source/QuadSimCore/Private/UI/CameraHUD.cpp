// Fill out your copyright notice in the Description page of Project Settings.
#include "Engine/Canvas.h"
#include "Pawns/QuadPawn.h"
#include "UI/CameraHUD.h"

void ACameraHUD::DrawHUD()
{
	Super::DrawHUD();

	// Lazy-grab the pawn’s render target
	if (!CachedFPVRT)
	{
		if (AQuadPawn* Q = Cast<AQuadPawn>(GetOwningPawn()))
		{
			CachedFPVRT = Q->RT_FPV;
		}
	}

	if (CachedFPVRT && Canvas)
	{
		const float margin = 10.f;
		const FVector2D size(200.f, 150.f);
		const FVector2D pos(Canvas->SizeX - size.X - margin, margin);

		Canvas->K2_DrawTexture(
		  CachedFPVRT,
		  pos,
		  size,
		  FVector2D(0.f,0.f),
		  FVector2D(1.f,1.f),
		  FLinearColor::White,
		  BLEND_Translucent
		);
	}
}
