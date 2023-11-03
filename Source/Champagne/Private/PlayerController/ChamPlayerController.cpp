// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ChamPlayerController.h"
#include "HUD/ChamHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void AChamPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChamHUD = Cast<AChamHUD>(GetHUD());
}

void AChamPlayerController::CheckHUD()
{
	ChamHUD = ChamHUD == nullptr ? Cast<AChamHUD>(GetHUD()) : ChamHUD;
}

void AChamPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	CheckHUD();

	if (ChamHUD && ChamHUD->CharacterOverlay && ChamHUD->CharacterOverlay->HealthPoint)
	{
		const float HealthPercent = Health / MaxHealth;
		ChamHUD->CharacterOverlay->HealthPoint->SetPercent(HealthPercent);
	}
}

void AChamPlayerController::SetCurrentArrows(const uint8 CurrentArrows)
{
	CheckHUD();

	if (ChamHUD && ChamHUD->CharacterOverlay && ChamHUD->CharacterOverlay->MaxArrows && ChamHUD->CharacterOverlay->CurrentArrows)
	{
		ChamHUD->CharacterOverlay->CurrentArrows->SetText(FText::AsNumber(CurrentArrows));
	}
}

void AChamPlayerController::SetMaxArrows(const uint8 MaxArrows)
{
	CheckHUD();

	if (ChamHUD && ChamHUD->CharacterOverlay && ChamHUD->CharacterOverlay->MaxArrows && ChamHUD->CharacterOverlay->CurrentArrows)
	{
		ChamHUD->CharacterOverlay->MaxArrows->SetText(FText::AsNumber(MaxArrows));
	}
}