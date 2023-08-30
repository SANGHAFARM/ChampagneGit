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
		//FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
		//FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());
		//MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		FVector Direction = UKismetMathLibrary::InverseTransformDirection(PlayerCharacter->GetActorTransform(), PlayerCharacter->GetVelocity());
		MoveDirection = UKismetMathLibrary::MakeRotFromX(Direction).Yaw;
		Pitch = PlayerCharacter->GetBaseAimRotation().Pitch;
	}	
}