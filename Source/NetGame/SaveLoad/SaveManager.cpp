// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"

#include "SaveGameData.h"
#include "SaveGameMetadata.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Define external symbols; "Unresolved external symbol compile error" otherwise
FString USaveManager::CurrentSaveSlot;
TArray<TScriptInterface<ISaveInterface>> USaveManager::SaveInterfaces;


void USaveManager::Init()
{
	CurrentSaveSlot = "_Development";

	const USaveGame* SaveGameMetadata = UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0);
	if (SaveGameMetadata == nullptr)
	{
		// Since the MetaData doesn't exist, create one
		USaveGame* TempSaveMetadata = UGameplayStatics::CreateSaveGameObject(USaveGameMetadata::StaticClass());
		UGameplayStatics::SaveGameToSlot(TempSaveMetadata, MetadataSaveSlot, 0);
	}
}

void USaveManager::QueryAllSaveInterfaces()
{
	// Clear old entries
	SaveInterfaces.Empty();

	// Get all actors that implement the intrerface
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), Actors);
	for (auto* Actor : Actors)
		SaveInterfaces.Add(Actor);
}

void USaveManager::SaveGame()
{
	// Create a new save game data instance
	USaveGameData* SaveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));

	QueryAllSaveInterfaces();
	// Go over all actors that need to be saved and save them
	for (auto& SaveInterface : SaveInterfaces)
	{
		if (SaveInterface.GetObject() == nullptr)
			continue;

		// Let Object know its about to be saved
		SaveInterface->Execute_OnBeforeSave(SaveInterface.GetObject());

		// Find the object's save data, using it's unique name
		FString UniqueSaveName = SaveInterface->Execute_GetUniqueSaveName(SaveInterface.GetObject());
		auto& [Data] = SaveGameData->SerialisedData.Add(UniqueSaveName);
		
		FMemoryWriter MemoryWriter = FMemoryWriter(Data);
		MemoryWriter.ArIsSaveGame = true;

		SaveInterface.GetObject()->Serialize(MemoryWriter);
	}

	// Saves the game to the current save slot
	UGameplayStatics::SaveGameToSlot(SaveGameData, CurrentSaveSlot, 0);

	// Update the save's metadata
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));
	auto& [SlotName, DateTime] = SaveGameMetadata->SavedGamesMetadata.FindOrAdd(CurrentSaveSlot);
	SlotName = CurrentSaveSlot;
	DateTime = FDateTime::Now();
	UGameplayStatics::SaveGameToSlot(SaveGameMetadata, MetadataSaveSlot, 0);

	if (GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Saved: " + CurrentSaveSlot);
}

void USaveManager::LoadGame()
{
	USaveGameData* SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
	if (SaveGameData == nullptr)
	{
		SaveGame();		// No Saves exist for this slot, so create a default one
		SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0)); // Reload it
	}

	// Loop over all actors that need to load data and load their data
	for (auto& SaveInterface : SaveInterfaces)
	{
		if (SaveInterface.GetObject() == nullptr)
			continue;

		// Find the object's save data, using it's unique name
		FString UniqueSaveName = SaveInterface->Execute_GetUniqueSaveName(SaveInterface.GetObject());
		FSaveData* SaveData = SaveGameData->SerialisedData.Find(UniqueSaveName);
		
		if (SaveData == nullptr)
			continue;
		
		FMemoryReader MemoryReader(SaveData->Data);
		MemoryReader.ArIsSaveGame = false;
		// Essentially load the data from the save
		SaveInterface.GetObject()->Serialize(MemoryReader);
	}
	
	if (GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Loaded: " + CurrentSaveSlot);

}

void USaveManager::DeleteSlot(const FString& SlotName)
{
	// Delete the actual GameData save file
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	// Update the MetaData file
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));
	SaveGameMetadata->SavedGamesMetadata.Remove(SlotName);
	UGameplayStatics::SaveGameToSlot(SaveGameMetadata, MetadataSaveSlot, 0);
}

FString USaveManager::GetNewSaveSlot(bool& bSlotFound)
{
	bSlotFound = false;

	// Load the Metadata file
	const USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));

	for (int i = 0; i < MaxSaveSlots; i++)
	{
		FString SlotName = "SaveSlot_" + FString::FromInt(i);
		if (!SaveGameMetadata->SavedGamesMetadata.Contains(SlotName))
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

TArray<FSaveMetadata> USaveManager::GetAllSaveMetadata()
{
	TArray<FSaveMetadata> Metadata;
	USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));

	// Allocate exact memory for array  
	Metadata.Reserve(SaveGameMetadata->SavedGamesMetadata.Num());
	// Add each save game's Metadata to the return array
	for (const auto& SavedGame : SaveGameMetadata->SavedGamesMetadata)
	{
		Metadata.Push(SavedGame.Value);
	}
	return Metadata;
}
