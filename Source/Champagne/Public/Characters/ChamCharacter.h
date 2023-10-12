// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ChamCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class AChamHUD;
class AChamPlayerController;
class UTexture2D;
class AArrow;


UCLASS()
class CHAMPAGNE_API AChamCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AChamCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Fire(const FVector& Hit);
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

	/** Aiming 버튼 입력에 따라 bAiming을 true 또는 false로 변경 */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** bAiming에 따라 조준 카메라 설정 */
	void CameraInterpZoom(float DeltaTime);

	void CalculateCrosshairSpread(float DeltaTime);

	void SetHUDCrosshairs(float DeltaTime);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:	
	AChamPlayerController* ChamController;

	/** <Camera> */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	/** 기본값 카메라 시야 */
	float CameraDefaultFOV;

	/** 조준 시 카메라 시야 */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV = 60.f;

	/** 현재 카메라 시야 */
	float CameraCurrentFOV;

	/** 조준 해제 시 카메라 축소 속도 */
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed = 20.f;

	/** 조준 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;
	/** </Camera> */

	/** <Dash> */
	UPROPERTY(VisibleAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector DashDirection;

	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float GroundDashDistance = 5000.f;

	bool bCanDash = true;

	FTimerHandle DashCoolTimer;

	void DashCoolTimerFinished();

	UPROPERTY(EditAnywhere, Category = Effect)
	UNiagaraComponent* DashEffect;

	FTimerHandle DashEffectTimer;
	
	float DashEffectTime = 0.5f;

	void DashEffectTimerFinished();
	/** </Dash> */

	/** <Crosshair> */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	float CrosshairVelocityFactor;

	float CrosshairInAirFactor;

	float CrosshairAimFactor;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairRight;
	/** </Crosshair> */

	AChamHUD* HUD;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArrow> ArrowClass;

	FVector HitTarget;

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

	FORCEINLINE void SetDashDirection(const FVector MoveDirection) { DashDirection = FVector(MoveDirection.X, MoveDirection.Y, 0.f); }
};