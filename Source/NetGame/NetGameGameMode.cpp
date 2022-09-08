// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetGameGameMode.h"
#include "SaveLoad/SaveManager.h"

ANetGameGameMode::ANetGameGameMode()
{
}

void ANetGameGameMode::InitGameState()
{
	Super::InitGameState();
}

APawn* ANetGameGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	APawn* Pawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);

	USaveManager::QueryAllSaveInterfaces();
	USaveManager::LoadGame();

	return Pawn;
}
