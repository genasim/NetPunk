// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameData.generated.h"


USTRUCT()
struct FSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<uint8> Data;
};


/**
 * 
 */
UCLASS()
class NETGAME_API USaveGameData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Save & Load")
	TMap<FString, FSaveData> SerialisedData;
};
