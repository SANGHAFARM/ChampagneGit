// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrappleHook/GrappleHookComponent.h"
#include "Weapon/GrappleHook/GrappleHook.h"
#include "Weapon/GrappleHook/GrappleCable.h"
#include "CableComponent.h"

// Sets default values for this component's properties
UGrappleHookComponent::UGrappleHookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UGrappleHookComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UGrappleHookComponent::IsInUse()
{
	if (GrappleState == EGrappleState::EGS_Firing || GrappleState == EGrappleState::EGS_AttachedToTarget)
		return true;
	else
		return false;
}

void UGrappleHookComponent::CancelGrapple()
{
	if (SpawnedGrappleHook && SpawnedGrappleCable)
	{
		SpawnedGrappleHook->Destroy();
		SpawnedGrappleHook = nullptr;

		SpawnedGrappleCable->Destroy();
		SpawnedGrappleCable = nullptr;

		GrappleState = EGrappleState::EGS_ReadyToFire;
	}
	
}

void UGrappleHookComponent::FireGrapple(FVector SpawnLocation, FRotator SpawnRotation)
{
	if (IsInUse() == false)
	{
		GrappleState = EGrappleState::EGS_Firing;

		if (GetWorld() && GrappleHook)
		{
			SpawnedGrappleHook = GetWorld()->SpawnActor<AGrappleHook>(GrappleHook, SpawnLocation, SpawnRotation);
			SpawnedGrappleCable = GetWorld()->SpawnActor<AGrappleCable>(GrappleCable, SpawnLocation, SpawnRotation);
			
			SpawnedGrappleCable->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
			SpawnedGrappleCable->CableComponent->SetAttachEndTo(SpawnedGrappleHook, FName(TEXT("GrappleHook")));
			SpawnedGrappleCable->CableComponent->EndLocation = FVector(0, 0, 0);
		}
	}
}
