// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrappleHook/GrappleHook.h"
#include "Weapon/GrappleHook/GrappleHookComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGrappleHook::AGrappleHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GrappleHook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrappleHook"));
	SetRootComponent(GrappleHook);
	GrappleHook->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));

	GrappleHookMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("GrappleHookMovement"));
	GrappleHookMovement->InitialSpeed = GrappleHookSpeed;
	GrappleHookMovement->MaxSpeed = GrappleHookSpeed;
}

// Called when the game starts or when spawned
void AGrappleHook::BeginPlay()
{
	Super::BeginPlay();

}

void AGrappleHook::StopHookMovement()
{
	GrappleHookMovement->Deactivate();
	GrappleHookMovement->Activate();
}

void AGrappleHook::SetHookVelocityZero()
{
	GrappleHookMovement->Velocity = FVector(0);
}
