// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ChamCharacter.h"
#include "PlayerController/ChamPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "Weapon/GrappleHook/GrappleHookComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Math/UnrealMathUtility.h"
#include "HUD/ChamHUD.h"
#include "Engine/Texture2D.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Arrow.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"


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

	GrappleHook = CreateDefaultSubobject<UGrappleHookComponent>(TEXT("GrappleHook"));		

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationYaw = false;

	DashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashEffect"));
	DashEffect->bAutoActivate = false;

	GetMesh()->bWaitForParallelClothTask = true;
	
}

// Called every frame
void AChamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 조준 시 카메라 확대/축소 속도 보간
	CameraInterpZoom(DeltaTime);

	SetHUDCrosshairs(DeltaTime);

	SetTargetArrowSpeed(DeltaTime);

	// 크로스헤어 퍼짐
	//CalculateCrosshairSpread(DeltaTime);

	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);
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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AChamCharacter::Interact);
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &AChamCharacter::TabOn);
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Completed, this, &AChamCharacter::TabOff);

		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Started, this, &AChamCharacter::Hook);


		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Started, this, &AChamCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(Aiming, ETriggerEvent::Completed, this, &AChamCharacter::AimingButtonReleased);
		EnhancedInputComponent->BindAction(Cancel, ETriggerEvent::Started, this, &AChamCharacter::AimingCancel);
	}
}

void AChamCharacter::SetOverlappingArrow(AArrow* Arrow)
{
	if (Arrow)
	{
		OverlappingArrows.Add(Arrow);
	}	
}

void AChamCharacter::RemoveOverlappingArrow(AArrow* Arrow)
{
	if (Arrow && OverlappingArrows.Contains(Arrow))
	{
		OverlappingArrows.Remove(Arrow);
	}
}

void AChamCharacter::CheckAndGetArrow(AArrow* Arrow)
{
	if (Arrow && OverlappingArrows.Contains(Arrow) && ArrowsInWorld.Contains(Arrow))
	{
		OverlappingArrows.Remove(Arrow);
		ArrowsInWorld.Remove(Arrow);

		Arrow->Destroy();
		Arrow = nullptr;

		if (CurrentArrows < MaxArrows && ChamController)
		{
			CurrentArrows++;
			ChamController->SetCurrentArrows(CurrentArrows);

			HideOrUnHideArrowMesh(CurrentArrows);
		}
	}
}

void AChamCharacter::Fire(const FVector& Hit)
{
	APawn* InstigatorPawn = Cast<APawn>(this);
	const USkeletalMeshSocket* ArrowSocket = GetMesh()->GetSocketByName(TEXT("arrow_socket"));

	if (ArrowSocket)
	{
		FTransform SocketTransform = ArrowSocket->GetSocketTransform(GetMesh());
		FVector ToTarget = Hit - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		if (ArrowClass && InstigatorPawn)
		{
			AArrow* DefaultArrow = Cast<AArrow>(ArrowClass.GetDefaultObject());

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();

			if (World && DefaultArrow)
			{
				float MaxSpeed = 1.0f;
				float NormalizeChargeTime = FMath::Clamp(ArrowTargetSpeed / MaxSpeed, 0.0f, 1.0f);
				float ArrowSpeed = ArrowInitialSpeed + (ArrowMaxSpeed - ArrowInitialSpeed) * NormalizeChargeTime;

				DefaultArrow->SetArrowSpeed(ArrowSpeed);
				
				AArrow* NewArrow = World->SpawnActor<AArrow>(
					ArrowClass, 
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);			

				if (NewArrow)
				{
					NewArrow->SetOwner(this);
					ArrowsInWorld.Add(NewArrow);
				}
			}
		}		
	}
}

// Called when the game starts or when spawned
void AChamCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentArrows = MaxArrows;

	ChamController = Cast<AChamPlayerController>(GetController());
	if (ChamController)
	{		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ChamController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}

		ChamController->SetHUDHealth(CurrentHealth, MaxHealth);
		ChamController->SetMaxArrows(MaxArrows);
		ChamController->SetCurrentArrows(CurrentArrows);
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
	//if (bCanDash == false || GetCharacterMovement()->IsFalling()) return;

	bCanDash = false;
	
	if (DashEffect)
	{
		DashEffect->Activate();

		LaunchCharacter(DashDirection * GroundDashDistance, true, false);
		
		GetWorldTimerManager().SetTimer(DashEffectTimer, this, &AChamCharacter::DashEffectTimerFinished, 0.2f);						
		GetWorldTimerManager().SetTimer(DashCoolTimer, this, &AChamCharacter::DashCoolTimerFinished, 2.f);
	}	
}

void AChamCharacter::Interact()
{	
	CheckAndGetArrow(SelectedArrow);
}

void AChamCharacter::TabOn()
{
	if (bFilterChanged == true)
		return;

	bFilterChanged = true;

	if (ScreenFilterMaterial && Camera)
	{
		Camera->PostProcessSettings.AddBlendable(ScreenFilterMaterial, 1.0f);
	}

	if (ArrowsInWorld.Num() > 0)
	{
		for (AArrow* Arrow : ArrowsInWorld)
		{
			Arrow->HighlightArrow(true);
		}
	}
}

void AChamCharacter::TabOff()
{
	if (bFilterChanged == false)
		return;

	bFilterChanged = false;

	if (ScreenFilterMaterial && Camera)
	{
		Camera->PostProcessSettings.RemoveBlendable(ScreenFilterMaterial);		
	}

	if (ArrowsInWorld.Num() > 0)
	{
		for (AArrow* Arrow : ArrowsInWorld)
		{
			Arrow->HighlightArrow(false);
		}
	}
}

void AChamCharacter::Hook()
{
	if (GrappleMontage)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		PlayMontageSection(GrappleMontage, TEXT("GrappleShot"));
	}
}

void AChamCharacter::HideOrUnHideArrowMesh(const uint8 CurArrows)
{
	if (GetMesh() == nullptr) return;

	if (CurArrows > 0)
	{
		GetMesh()->UnHideBoneByName(FName("arrow_nock"));
	}
	else
	{
		GetMesh()->HideBoneByName(FName("arrow_nock"), EPhysBodyOp::PBO_None);
	}
}

void AChamCharacter::AimingButtonPressed()
{
	if (Camera && bCanFire && CurrentArrows > 0)
	{
		bAiming = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;	

		TabOff();
	}	
}

void AChamCharacter::AimingButtonReleased()
{
	if (Camera && bAiming && FireMontage)
	{
		Fire(HitTarget);
		PlayMontageSection(FireMontage, TEXT("Reload"));

		if (CurrentArrows > 0 && ChamController)
		{
			CurrentArrows--;
			ChamController->SetCurrentArrows(CurrentArrows);

			HideOrUnHideArrowMesh(CurrentArrows);
		}

		bAiming = false;	
		bCanFire = false;
		ArrowTargetSpeed = 0.f;

		if (GetCharacterMovement()->Velocity.Size() <= 0)
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
		}	
	}	
}

void AChamCharacter::AimingCancel()
{
	if (Camera && bAiming)
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
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);		
	}

	if (Camera)
	{
		GetCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

void AChamCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, GetCharacterMovement()->MaxWalkSpeed};
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

void AChamCharacter::SetHUDCrosshairs(float DeltaTime)
{
	if (ChamController == nullptr) return;

	if (ChamController)
	{
		HUD = HUD == nullptr ? Cast<AChamHUD>(ChamController->GetHUD()) : HUD;
		UWorld* World = GetWorld();

		if (HUD && World)
		{
			FHUDPackage HUDPackage;
			HUDPackage.CrosshairCenter = CrosshairCenter;
			HUDPackage.CrosshairLeft = CrosshairLeft;
			HUDPackage.CrosshairRight = CrosshairRight;

			// 크로스헤어 퍼짐 계산
			FVector2D WalkSpeedRange{ 0.f, GetCharacterMovement()->MaxWalkSpeed };
			FVector2D VelocityMultiplierRange{ 0.f, 1.f };
			FVector Velocity = GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (GetCharacterMovement()->IsFalling() || World->GetTimerManager().IsTimerActive(DashEffectTimer))
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::InterpExpoIn(CrosshairAimFactor, 0.95f, 0.5f);
				//FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, )
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			HUDPackage.CrosshairSpread = 1.f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void AChamCharacter::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * 15000.f;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			AArrow* ArrowActor = Cast<AArrow>(TraceHitResult.GetActor());

			TracingArrow(ArrowActor);

			HitTarget = TraceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12, FColor::Red);
			//UE_LOG(LogTemp, Warning, TEXT("Actor Name : %s"), *TraceHitResult.GetActor()->GetName());
		}
	}
}

void AChamCharacter::TracingArrow(AArrow* TracingArrow)
{
	if (TracingArrow && OverlappingArrows.Contains(TracingArrow) && SelectedArrow == nullptr)
	{
		SelectedArrow = TracingArrow;

		IPickUpInterface* ArrowInterface = Cast<IPickUpInterface>(SelectedArrow);
		ArrowInterface->HighlightArrow(true);
		ArrowInterface->ShowPickupWidget(true);
	}
	else if (SelectedArrow && SelectedArrow != TracingArrow)
	{
		IPickUpInterface* ArrowInterface = Cast<IPickUpInterface>(SelectedArrow);

		if (bFilterChanged == false)
		{
			ArrowInterface->HighlightArrow(false);
		}

		ArrowInterface->ShowPickupWidget(false);

		SelectedArrow = nullptr;
	}
}

void AChamCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AChamCharacter::SetCanFire()
{
	if (bCanFire == false)
	{
		bCanFire = true;
	}
}

void AChamCharacter::SetTargetArrowSpeed(float DeltaTime)
{
	if (bAiming)
	{
		ArrowTargetSpeed += DeltaTime * 1.5f;		
	}
}

void AChamCharacter::DashCoolTimerFinished()
{
	bCanDash = true;
}

void AChamCharacter::DashEffectTimerFinished()
{
	DashEffect->Deactivate();
}

void AChamCharacter::PlayFireGrapple()
{
	const USkeletalMeshSocket* ArrowSocket = GetMesh()->GetSocketByName(TEXT("arrow_socket"));

	if (ArrowSocket)
	{
		FVector SocketLocation = ArrowSocket->GetSocketLocation(GetMesh());
		FVector ToTarget = HitTarget - SocketLocation;
		FRotator TargetRotation = ToTarget.Rotation();

		GrappleHook->FireGrapple(SocketLocation, TargetRotation);
	}

	if (GetCharacterMovement()->Velocity.Size() <= 0)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

