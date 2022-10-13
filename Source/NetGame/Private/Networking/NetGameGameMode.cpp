// Copyright Epic Games, Inc. All Rights Reserved.

#include "Networking/NetGameGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/NetPlayerController.h"
#include "SaveLoad/SaveManager.h"

ANetGameGameMode::ANetGameGameMode()
{
	PlayerControllerClass = ANetPlayerController::StaticClass();
}

APawn* ANetGameGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	APawn* Pawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);
	
	USaveManager::QueryAllSaveInterfaces();
	const TArray<TObjectPtr<APlayerState>> PlayerStates = GameState->PlayerArray;
	
	if (NewPlayer == PlayerStates[0]->GetPlayerController())
		USaveManager::LoadGame();
	else
		// If character is not that of the host -> spawn at host's location
		Pawn->SetActorLocation(PlayerStates[0]->GetPawn()->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);

	return Pawn;
}
