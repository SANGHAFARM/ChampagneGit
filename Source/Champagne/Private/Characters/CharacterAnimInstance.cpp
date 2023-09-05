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
		// 캐릭터가 현재 바라보는 방향의 회전
		FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();

		// 캐릭터가 현재 이동중인 방향의 회전
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());

		// 캐릭터가 이동중인 방향 회전과 캐릭터가 바라보는 방향 회전의 차이값의 Yaw를 통해
		// 캐릭터가 현재 바라보는 방향의 회전 0도를 기준으로, 이동하려는 회전값을 지정
		MoveDirection = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		Pitch = AimRotation.Pitch;

		// 대시 방향 지정
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
