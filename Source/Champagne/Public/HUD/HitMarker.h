// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitMarker.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class CHAMPAGNE_API UHitMarker : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* HitMark;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HitMarkFade;

	UPROPERTY(meta = (BindWidget))
	UImage* DeathMark;


	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* DeathMarkFade;

	void RemoveHitMarkAnim();
};
