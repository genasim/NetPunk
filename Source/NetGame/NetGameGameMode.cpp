// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetGameGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "SaveLoad/SaveManager.h"

ANetGameGameMode::ANetGameGameMode() {}

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
	
	// If character is not that of the host -> spawn at host's location
	const TArray<TObjectPtr<APlayerState>> PlayerStates = GameState->PlayerArray;
	if (PlayerStates.Num() > 1)
		Pawn->SetActorLocation(PlayerStates[0]->GetPawn()->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);

	return Pawn;
}
