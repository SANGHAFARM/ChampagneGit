// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ChamCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraComponent.h"

// Sets default values
AChamCharacter::AChamCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 스프링 암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	SpringArm->SocketOffset.Y = 100.f;

	// 캐릭터 카메라 생성 및 스프링 암에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->FieldOfView = 90.f;
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationYaw = false;

	DashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashEffect"));
	DashEffect->SetupAttachment(GetRootComponent());
	DashEffect->bAutoActivate = false;
	
}

// Called every frame
void AChamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 조준 시 카메라 확대/축소 속도 보간
	CameraInterpZoom(DeltaTime);

	// 크로스헤어 퍼짐
	CalculateCrosshairSpread(DeltaTime);

}

// Called to bind functionality to input
void AChamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AChamCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AChamCharacter::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AChamCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AChamCharacter::Dash);

		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Ongoing, this, &AChamCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Completed, this, &AChamCharacter::AimingButtonReleased);
	}
}

float AChamCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

// Called when the game starts or when spawned
void AChamCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

	if (Camera)
	{
		CameraDefaultFOV = Camera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}	
}

void AChamCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MoveVector.Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MoveVector.X);		
	}
}

void AChamCharacter::MoveEnd()
{
	if (Controller && bAiming == false)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void AChamCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	
	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AChamCharacter::Dash()
{
	if (bCanDash == false) return;

	bCanDash = false;
	
	if (DashEffect)
	{
		DashEffect->Activate();

		if (GetCharacterMovement()->IsFalling())
		{
			LaunchCharacter(DashDirection * AirDashDistance, true, false);
		}
		else
		{
			LaunchCharacter(DashDirection * GroundDashDistance, true, false);
		}
		
		GetWorldTimerManager().SetTimer(DashEffectTimer, this, &AChamCharacter::DashEffectTimerFinished, 0.2f);						
		GetWorldTimerManager().SetTimer(DashCoolTimer, this, &AChamCharacter::DashCoolTimerFinished, 2.f);
	}	
}

void AChamCharacter::AimingButtonPressed()
{
	if (Camera)
	{
		bAiming = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}	
}

void AChamCharacter::AimingButtonReleased()
{
	if (Camera)
	{
		bAiming = false;

		if (GetCharacterMovement()->Velocity.Size() <= 0)
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}		
	}	
}

void AChamCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{		
		CameraCurrentFOV = FMath::InterpExpoIn(CameraCurrentFOV, CameraZoomedFOV, 0.5f);
		//CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
		GetCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
		GetCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

void AChamCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

void AChamCharacter::DashCoolTimerFinished()
{
	bCanDash = true;
}

void AChamCharacter::DashEffectTimerFinished()
{
	DashEffect->Deactivate();
}

