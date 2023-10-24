// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PickUpInterface.h"
#include "Arrow.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UBoxComponent;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class CHAMPAGNE_API AArrow : public AActor, public IPickUpInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();

	virtual void HighlightArrow() override;
	virtual void UnHighlightArrow() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void WhenHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereArea;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitParticle;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraComponent* ArrowTrail;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ArrowMovement;

public:
	void SetArrowSpeed(float ArrowSpeed);
		
};
