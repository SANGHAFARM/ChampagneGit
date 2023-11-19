// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChamPlayerController.generated.h"

class AChamHUD;

/**
 * 
 */
UCLASS()
class CHAMPAGNE_API AChamPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void CheckHUD();

	void SetHUDHealth(float Health, float MaxHealth);	
	void SetCurrentArrows(const uint8 CurrentArrows);
	void SetMaxArrows(const uint8 MaxArrows);

	void PlayHitMarker(bool bIsAlive);

protected:
	virtual void BeginPlay() override;

private:
	AChamHUD* ChamHUD;
};
