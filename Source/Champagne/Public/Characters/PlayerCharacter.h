// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;


UCLASS()
class CHAMPAGNE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CharacterMoveDirection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** <Enhanced Input> */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Aiming;
	/** </Enhanced Input> */

	void Move(const FInputActionValue& Value);
	void MoveEnd();
	void Look(const FInputActionValue& Value);
	void Dash();

	/** Aiming ��ư �Է¿� ���� bAiming�� true �Ǵ� false�� ���� */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** bAiming�� ���� ���� ī�޶� ���� */
	void CameraInterpZoom(float DeltaTime);

private:	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	/** �⺻ ī�޶� �þ� */
	float CameraDefaultFOV;

	/** ���� �� ī�޶� �þ� */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV = 60.f;

	/** ���� ī�޶� �þ� */
	float CameraCurrentFOV;

	/** ���� �� ī�޶� Ȯ�� �ӵ� */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed = 20.f;

	/** ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;

	UPROPERTY(VisibleAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector DashDirection;

	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraComponent* DashEffect;

	FTimerHandle DashEffectTimer;

	void DashEffectTimerFinished();

	float DashEffectTime = 0.5f;

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE void SetDashDirection(FVector MoveDirection) { DashDirection = FVector(MoveDirection.X, MoveDirection.Y, 0.f); }	
};