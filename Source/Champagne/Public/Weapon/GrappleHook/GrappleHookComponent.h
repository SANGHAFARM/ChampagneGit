// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleHookComponent.generated.h"

UENUM(BlueprintType)
enum class EGrappleState : uint8
{
	EGS_ReadyToFire UMETA(DisplayName = "ReadyToFire"),
	EGS_Firing UMETA(DisplayName = "Firing"),
	EGS_AttachedToTarget UMETA(DisplayName = "AttachedToTarget"),
};

class AGrappleHook;
class AGrappleCable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHAMPAGNE_API UGrappleHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleHookComponent();

	void FireGrapple(FVector SpawnLocation, FRotator SpawnRotation);

	void CancelGrapple();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool IsInUse();

private:	
	UPROPERTY(VisibleAnywhere)
	EGrappleState GrappleState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGrappleHook> GrappleHook;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGrappleCable> GrappleCable;

	UPROPERTY(VisibleAnywhere)
	AGrappleHook* SpawnedGrappleHook;

	UPROPERTY(VisibleAnywhere)
	AGrappleCable* SpawnedGrappleCable;
};
