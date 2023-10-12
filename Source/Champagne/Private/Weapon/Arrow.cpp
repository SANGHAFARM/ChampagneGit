// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Arrow.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	Root->bVisualizeComponent = true;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(GetRootComponent());
	ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ArrowMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ArrowMovement"));
	ArrowMovement->bRotationFollowsVelocity = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(ArrowMesh);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

// Called every frame
void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);
}

void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		ArrowMovement->StopMovementImmediately();
		ArrowMovement->ProjectileGravityScale = 0.f;
		AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FVector ParticleLocation = BoxCollision->GetComponentToWorld().GetLocation();
		UWorld* World = GetWorld();
		if (World && HitParticle)
		{			
			UGameplayStatics::SpawnEmitterAtLocation(World, HitParticle, ParticleLocation);
		}
	}
}
