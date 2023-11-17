// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleHook.generated.h"

class UProjectileMovementComponent;
class ACableActor;

UCLASS()
class CHAMPAGNE_API AGrappleHook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrappleHook();

	void StopHookMovement();

	void SetHookVelocityZero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* GrappleHook;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* GrappleHookMovement;

	UPROPERTY(EditAnywhere)
	float GrappleHookSpeed = 5000.f;

public:
	FORCEINLINE UStaticMeshComponent* GetHook() const { return GrappleHook; }
	
};
