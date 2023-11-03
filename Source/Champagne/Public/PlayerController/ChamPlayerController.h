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
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;

private:
	AChamHUD* ChamHUD;
};
