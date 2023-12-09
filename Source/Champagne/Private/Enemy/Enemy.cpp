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

	WeakPointMark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeakPointMesh"));
	WeakPointMark->SetupAttachment(GetRootComponent());
	WeakPointMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	if (WeakPointMark)
	{
		WeakPointMark->SetHiddenInGame(true);
	}	
	
	if (!ValidWeakBones.IsEmpty())
	{
		int32 RandIdx = FMath::RandRange(0, ValidWeakBones.Num() - 1);
		WeakBone = ValidWeakBones[RandIdx];
		WeakBoneIdx = GetMesh()->GetBoneIndex(WeakBone);
				
		if (WeakBone.IsValid())
		{
			WeakPointMark->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeakBone);
		}		
	}
}

void AEnemy::ShowWeakPoint(bool bShow)
{
	if (WeakPointMark)
	{
		if (bShow)
		{
			WeakPointMark->SetHiddenInGame(false);
			WeakPointMark->SetRenderCustomDepth(true);
			WeakPointMark->SetCustomDepthStencilValue(2);
		}
		else
		{
			WeakPointMark->SetRenderCustomDepth(false);
			WeakPointMark->SetHiddenInGame(true);
		}
	}
}