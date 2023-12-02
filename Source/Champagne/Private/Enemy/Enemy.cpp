// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
//#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ValidWeakBones.IsEmpty())
	{
		int32 RandIdx = FMath::RandRange(0, ValidWeakBones.Num() - 1);
		WeakBone = ValidWeakBones[RandIdx];
		
		if (WeakBone.IsValid())
		{
			WeakPoint = UNiagaraFunctionLibrary::SpawnSystemAttached(
				WeakPointEffect,
				GetMesh(),
				WeakBone,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				false,
				false
			);
		}		
	}
}

void AEnemy::ShowWeakPoint(bool bShow)
{
	if (WeakPoint)
	{
		if (bShow)
		{
			WeakPoint->Activate();
		}
		else
		{
			WeakPoint->Deactivate();
		}
	}
}