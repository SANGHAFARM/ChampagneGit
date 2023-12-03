// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Enemyinterface.h"
#include "Enemy.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class CHAMPAGNE_API AEnemy : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void ShowWeakPoint(bool bShow) override;

private:
	UPROPERTY(EditAnywhere)
	TArray<FName> ValidWeakBones;

	UPROPERTY(VisibleAnywhere)
	FName WeakBone;

	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraSystem* WeakPointEffect;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UNiagaraComponent* WeakPoint;

public:
	FORCEINLINE FName GetWeakBone() { return WeakBone; }
};
