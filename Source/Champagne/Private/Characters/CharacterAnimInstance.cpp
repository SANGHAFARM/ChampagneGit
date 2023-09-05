// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterAnimInstance.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
		IsFalling = PlayerCharacterMovement->IsFalling();
	}

	if (PlayerCharacter)
	{
		// ĳ���Ͱ� ���� �ٶ󺸴� ������ ȸ��
		FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();

		// ĳ���Ͱ� ���� �̵����� ������ ȸ��
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());

		// ĳ���Ͱ� �̵����� ���� ȸ���� ĳ���Ͱ� �ٶ󺸴� ���� ȸ���� ���̰��� Yaw�� ����
		// ĳ���Ͱ� ���� �ٶ󺸴� ������ ȸ�� 0���� ��������, �̵��Ϸ��� ȸ������ ����
		MoveDirection = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		Pitch = AimRotation.Pitch;

		// ��� ���� ����
		PlayerCharacter->SetDashDirection(MovementRotation.Vector());

		bAiming = PlayerCharacter->GetAiming();

		TurnInPlace();



		//FVector Direction = UKismetMathLibrary::InverseTransformDirection(PlayerCharacter->GetActorTransform(), PlayerCharacter->GetVelocity());
		//MoveDirection = UKismetMathLibrary::MakeRotFromX(Direction).Yaw;		
		

		//FVector MoveDirection = (UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity)).Vector();
		//DashDirection = FVector(MoveDirection.X, MoveDirection.Y, 0.f);		
	}	
}

void UCharacterAnimInstance::TurnInPlace()
{
	if (PlayerCharacter == nullptr) return;

	if (GroundSpeed <= 0 && bAiming)
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = PlayerCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastFrame };

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);
	}
}
