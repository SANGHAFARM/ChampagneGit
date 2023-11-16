// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrappleHook/GrappleHook.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGrappleHook::AGrappleHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	
	GrappleHook->OnComponentHit.AddDynamic(this, &AGrappleHook::OnGrappleHit);
}

void AGrappleHook::OnGrappleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

