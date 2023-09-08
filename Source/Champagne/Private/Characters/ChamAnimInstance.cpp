// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ChamAnimInstance.h"
#include "Characters/ChamCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UChamAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AChamCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UChamAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
		IsFalling = PlayerCharacterMovement->IsFalling();
		Velocity = PlayerCharacterMovement->Velocity;
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
		if (GroundSpeed <= 0)
		{
			PlayerCharacter->SetDashDirection(PlayerCharacter->GetActorForwardVector());
		}
		else
		{
			PlayerCharacter->SetDashDirection(MovementRotation.Vector());
		}		

		bAiming = PlayerCharacter->GetAiming();




		//FVector Direction = UKismetMathLibrary::InverseTransformDirection(PlayerCharacter->GetActorTransform(), PlayerCharacter->GetVelocity());
		//MoveDirection = UKismetMathLibrary::MakeRotFromX(Direction).Yaw;		
		

		//FVector MoveDirection = (UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity)).Vector();
		//DashDirection = FVector(MoveDirection.X, MoveDirection.Y, 0.f);		
	}	
}
