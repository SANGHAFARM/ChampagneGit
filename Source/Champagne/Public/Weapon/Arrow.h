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
class UWidgetComponent;

UCLASS()
class CHAMPAGNE_API AArrow : public AActor, public IPickUpInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();

	virtual void HighlightArrow(bool Switch) override;

	virtual void ShowPickupWidget(bool Switch) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void WhenHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BoneHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnSphereAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowprivateAccess = "True"))
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ArrowBox;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereArea;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitParticle;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraComponent* ArrowTrail;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ArrowMovement;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* PickupWidget;

public:
	void SetArrowSpeed(float ArrowSpeed);
		
};
