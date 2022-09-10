// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveMetadata.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameMetadata.generated.h"

/**
 *  @brief Holds the MetaData for all SaveSlots 
 */
UCLASS()
class NETGAME_API USaveGameMetadata : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Save & Load")
	TMap<FString, FSaveMetadata> SavedGamesMetadata;
};
