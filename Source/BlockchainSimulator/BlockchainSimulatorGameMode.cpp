// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlockchainSimulatorGameMode.h"
#include "BlockchainSimulatorCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABlockchainSimulatorGameMode::ABlockchainSimulatorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
