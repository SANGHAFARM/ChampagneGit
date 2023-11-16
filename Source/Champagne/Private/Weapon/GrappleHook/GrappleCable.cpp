// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrappleHook/GrappleCable.h"
#include "CableComponent.h"

AGrappleCable::AGrappleCable()
{
	CableComponent->bEnableStiffness = true;
	CableComponent->SubstepTime = 0.005f;
}
