// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"

#include "SaveGameData.h"
#include "SaveGameMetadata.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Define external symbol; "Unresolved external symbol compile error" otherwise
FString USaveManager::CurrentSaveSlot;

void USaveManager::Init()
{
	CurrentSaveSlot = "DefaultSlot";

	// Make sure MetaData file exists if the game has never been launched before
	USaveGame* SaveGameMetaData = UGameplayStatics::LoadGameFromSlot(MetaDataSaveSlot, 0);
	if (!SaveGameMetaData)
	{
		// Since the MetaData doesn't exist, create one
		USaveGame* tempSaveMetaData = UGameplayStatics::CreateSaveGameObject(USaveGameMetadata::StaticClass());
		UGameplayStatics::SaveGameToSlot(tempSaveMetaData, MetaDataSaveSlot, 0);
	}
}

void USaveManager::QueryAllSaveInterfaces()
{
}

void USaveManager::SaveGame()
{
	// Create a new save game data instance
	USaveGameData* SaveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	// TODO: Go over all actors that need to be saved and save them

	// Saves the game to the current save slot
	UGameplayStatics::SaveGameToSlot(SaveGameData, CurrentSaveSlot, 0);

	// Update the save's metadata
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetaDataSaveSlot, 0));
	// todo: remove comments
	// FSaveMetaData& SaveMetaData = SaveGameMetaData->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	// SaveMetaData.SlotName = CurrentSaveSlot;
	// SaveMetaData.DateTime = FDateTime::Now();
	auto& [SlotName, DateTime] = SaveGameMetadata->SavedGamesMetaData.FindOrAdd(CurrentSaveSlot);
	SlotName = CurrentSaveSlot;
	DateTime = FDateTime::Now();
	UGameplayStatics::SaveGameToSlot(SaveGameMetadata, MetaDataSaveSlot, 0);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Saved: " + CurrentSaveSlot);
}

void USaveManager::LoadGame()
{
	USaveGameData* SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
	if (!SaveGameData)
	{
		SaveGame();		// No Saves exist for this slot, so create a default one
		// todo: remove comments
		SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
		// LoadGame();
	}

	// TODO: Loop over all actors that need to load data and load their data
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Loaded: " + CurrentSaveSlot);

}

void USaveManager::DeleteSlot(const FString& SlotName)
{
	// Delete the actual GameData save file
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	// Update the MetaData file
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetaDataSaveSlot, 0));
	SaveGameMetadata->SavedGamesMetaData.Remove(SlotName);
	UGameplayStatics::SaveGameToSlot(SaveGameMetadata, MetaDataSaveSlot, 0);
}

FString USaveManager::GetNewSaveSlot(bool& bSlotFound)
{
	bSlotFound = false;

	// Load the Metadata file
	const USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetaDataSaveSlot, 0));

	for (int i = 0; i < MaxServerSlots; i++)
	{
		FString SlotName = "SaveSlot_" + FString::FromInt(i);
		if (!SaveGameMetadata->SavedGamesMetaData.Contains(SlotName))
		{
			bSlotFound = true;
			return SlotName;
		}
	}
	return FString();
}

FString USaveManager::GetCurrentSaveSlot()
{
	return CurrentSaveSlot;
}

void USaveManager::SetCurrentSaveSlot(const FString& SlotName)
{
	CurrentSaveSlot = SlotName;
}

TArray<FSaveMetadata> USaveManager::GetAllSaveMetaData()
{
	TArray<FSaveMetadata> Metadata;
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetaDataSaveSlot, 0));

	// Allocate exact memory for array  
	Metadata.Reserve(SaveGameMetadata->SavedGamesMetaData.Num());
	// Add each save game's Metadata to the return array
	for (const auto& SavedGame : SaveGameMetadata->SavedGamesMetaData)
	{
		Metadata.Push(SavedGame.Value);
	}
	return Metadata;
}
