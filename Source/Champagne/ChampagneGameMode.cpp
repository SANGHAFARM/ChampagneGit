// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChampagneGameMode.h"
#include "ChampagneCharacter.h"
#include "UObject/ConstructorHelpers.h"

AChampagneGameMode::AChampagneGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
