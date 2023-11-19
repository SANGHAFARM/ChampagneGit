// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HitMarker.h"
#include "Animation/WidgetAnimation.h"

void UHitMarker::RemoveHitMarkAnim()
{
	if (HitMarkFade && DeathMarkFade)
	{
		HitMarkFade->RemoveFromRoot();
		DeathMarkFade->RemoveFromRoot();
	}
}