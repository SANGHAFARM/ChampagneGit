// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ChamPlayerController.h"
#include "HUD/ChamHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"

void AChamPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChamHUD = Cast<AChamHUD>(GetHUD());
}

void AChamPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ChamHUD = ChamHUD == nullptr ? Cast<AChamHUD>(GetHUD()) : ChamHUD;

	if (ChamHUD && ChamHUD->CharacterOverlay && ChamHUD->CharacterOverlay->HealthPoint)
	{
		const float HealthPercent = Health / MaxHealth;
		ChamHUD->CharacterOverlay->HealthPoint->SetPercent(HealthPercent);
	}
}
