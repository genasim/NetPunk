// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetGameGameMode.h"
#include "SaveLoad/SaveManager.h"
#include "UObject/ConstructorHelpers.h"

ANetGameGameMode::ANetGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Players/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ANetGameGameMode::InitGameState()
{
	USaveManager::QueryAllSaveInterfaces();
	USaveManager::LoadGame();

	Super::InitGameState();
}
