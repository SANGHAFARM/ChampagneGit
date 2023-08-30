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
	UInputAction* Aiming;
	/** </Enhanced Input> */

	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/** Aiming 버튼 입력에 따라 bAiming을 true 또는 false로 변경 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** bAiming에 따라 조준 카메라 설정 */
	void CameraInterpZoom(float DeltaTime);

private:	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	/** 기본 카메라 시야 */
	float CameraDefaultFOV;

	/** 조준 시 카메라 시야 */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV = 60.f;

	/** 현재 카메라 시야 */
	float CameraCurrentFOV;

	/** 조준 시 카메라 확대 속도 */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed = 20.f;

	/** 조준 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
};