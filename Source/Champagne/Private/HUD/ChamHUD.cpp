// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ChamHUD.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/HitMarker.h"

void AChamHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairCenter)
		{
			FVector2D Spread(0.f);
			DrawCrosshair(HUDPackage.CrosshairCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}

		if (HUDPackage.CrosshairRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

void AChamHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AChamHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AChamHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor)
{
	const float TextureWidth = Texture->GetSizeX() + 5.f;
	const float TextureHeight = Texture->GetSizeY() + 5.f;
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairsColor
	);
}

void AChamHUD::AddHitMarker()
{
	if (HitMarkerClass)
	{
		UHitMarker* NewHitMarker = CreateWidget<UHitMarker>(GetOwningPlayerController(), HitMarkerClass);

		if (NewHitMarker)
		{
			NewHitMarker->AddToViewport();
			NewHitMarker->PlayAnimation(NewHitMarker->HitMarkFade);
			NewHitMarker->RemoveHitMarkAnim();
		}
	}
}

void AChamHUD::AddDeathMarker()
{
	if (HitMarkerClass)
	{
		UHitMarker* NewHitMarker = CreateWidget<UHitMarker>(GetOwningPlayerController(), HitMarkerClass);

		if (NewHitMarker)
		{
			NewHitMarker->AddToViewport();
			NewHitMarker->PlayAnimation(NewHitMarker->DeathMarkFade);
			NewHitMarker->RemoveHitMarkAnim();
		}
	}
}
