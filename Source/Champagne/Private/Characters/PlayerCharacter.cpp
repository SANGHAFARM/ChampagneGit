// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 스프링 암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 200.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	SpringArm->SocketOffset.Y = 100.f;

	// 캐릭터 카메라 생성 및 스프링 암에 부착
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
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
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);

	//FVector MoveDirection = (UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity)).Vector();
	//DashDirection = FVector(MoveDirection.X, MoveDirection.Y, 0.f);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dash);

		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Ongoing, this, &APlayerCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Completed, this, &APlayerCharacter::AimingButtonReleased);
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
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

void APlayerCharacter::Move(const FInputActionValue& Value)
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

void APlayerCharacter::MoveEnd()
{
	if (Controller)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	
	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void APlayerCharacter::Dash()
{
	if (DashEffect)
	{
		DashEffect->Activate();
		GetWorldTimerManager().SetTimer(DashEffectTimer, this, &APlayerCharacter::DashEffectTimerFinished, 0.08f);						
	}	

	LaunchCharacter(DashDirection * DashDistance, true, false);
}

void APlayerCharacter::AimingButtonPressed()
{
	if (Camera)
	{
		bAiming = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}	
}

void APlayerCharacter::AimingButtonReleased()
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

void APlayerCharacter::CameraInterpZoom(float DeltaTime)
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

void APlayerCharacter::DashEffectTimerFinished()
{
	if (DashEffect)
	{
		DashEffect->Deactivate();
	}
}

