// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrappleHook/GrappleHookComponent.h"
#include "Weapon/GrappleHook/GrappleHook.h"
#include "Weapon/GrappleHook/GrappleCable.h"
#include "CableComponent.h"
#include "Weapon/Arrow.h"
#include "Characters/ChamCharacter.h"

#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UGrappleHookComponent::UGrappleHookComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGrappleHookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SpawnedGrappleHook)
	{
		FVector CableStartLocation = GetOwner()->FindComponentByClass<USkeletalMeshComponent>()->GetSocketLocation(TEXT("arrow_anchor"));

		if (GrappleState == EGrappleState::EGS_AttachedToTarget)
		{			
			FVector ComebackLocation = FMath::VInterpConstantTo(SpawnedGrappleHook->GetActorLocation(), CableStartLocation, DeltaTime, 3000.f);
			SpawnedGrappleHook->SetActorLocation(ComebackLocation);

			if (FVector::Distance(CableStartLocation, SpawnedGrappleHook->GetActorLocation()) < 100.f)
			{				
				CancelGrapple();
			}
		}
		else if (FVector::Distance(CableStartLocation, SpawnedGrappleHook->GetActorLocation()) > 1500.f)
		{
			GrappleState = EGrappleState::EGS_AttachedToTarget;
			SpawnedGrappleHook->SetHookVelocityZero();
		}		
	}
}

// Called when the game starts
void UGrappleHookComponent::BeginPlay()
{
	Super::BeginPlay();
		
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
		CheckCatchedArrow();

		SpawnedGrappleHook->Destroy();
		SpawnedGrappleHook = nullptr;

		SpawnedGrappleCable->Destroy();
		SpawnedGrappleCable = nullptr;

		GrappleState = EGrappleState::EGS_ReadyToFire;
	}	
}

void UGrappleHookComponent::CheckCatchedArrow()
{
	if (SpawnedGrappleHook)
	{
		TArray<AActor*> AttachedActors;
		SpawnedGrappleHook->GetAttachedActors(AttachedActors);

		for (AActor* AttacedActor : AttachedActors)
		{
			AArrow* CatchedArrow = Cast<AArrow>(AttacedActor);
			AChamCharacter* ChamCharacter = Cast<AChamCharacter>(GetOwner());

			if (CatchedArrow && ChamCharacter)
			{
				ChamCharacter->CheckAndGetArrow(CatchedArrow);
			}
		}
	}
}

void UGrappleHookComponent::OnGrappleHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GrappleState = EGrappleState::EGS_AttachedToTarget;

	if (OtherActor != SpawnedGrappleCable && OtherActor != SpawnedGrappleHook)
	{
		AArrow* CatchedArrow = Cast<AArrow>(OtherActor);

		AEnemy* Enemy = Cast<AEnemy>(OtherActor);

		if (CatchedArrow)
		{
			UE_LOG(LogTemp, Warning, TEXT("Arrow Hit!"));
			CatchedArrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CatchedArrow->AttachToActor(SpawnedGrappleHook, FAttachmentTransformRules::KeepWorldTransform);
		}
		else if (Enemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("BoneName : %s"), *Hit.BoneName.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Component : %s"), *Hit.Component.Get()->GetName());
		}
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
						
			SpawnedGrappleCable->AttachToComponent(GetOwner()->FindComponentByClass<USkeletalMeshComponent>(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("arrow_anchor"));			
			SpawnedGrappleCable->CableComponent->SetAttachEndTo(SpawnedGrappleHook, FName(TEXT("GrappleHook")));
			SpawnedGrappleCable->CableComponent->EndLocation = FVector(0, 0, 0);

			if (SpawnedGrappleHook)
			{
				SpawnedGrappleHook->GetHook()->OnComponentHit.AddDynamic(this, &UGrappleHookComponent::OnGrappleHookHit);
			}
		}
	}
}
