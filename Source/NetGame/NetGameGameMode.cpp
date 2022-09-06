// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetGameGameMode.h"
#include "SaveLoad/SaveManager.h"

ANetGameGameMode::ANetGameGameMode()
{
}

void ANetGameGameMode::InitGameState()
{
	USaveManager::QueryAllSaveInterfaces();
	USaveManager::LoadGame();
	
	Super::InitGameState();
}
