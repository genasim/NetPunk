// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"

#include "SaveGameData.h"
#include "SaveGameMetadata.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "NetGame/Networking/NWGameInstance.h"

// Define external symbols; "Unresolved external symbol compile error" otherwise
TArray<TScriptInterface<ISaveInterface>> USaveManager::SaveInterfaces;
FString USaveManager::CurrentSaveSlot;
UNWGameInstance* USaveManager::GameInstance;

void USaveManager::Init(UNWGameInstance* InGameInstance)
{
	CurrentSaveSlot = "_Development";
	GameInstance = InGameInstance;
	ClearSaveInterfaceArray();
	
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
	ClearSaveInterfaceArray();

	// Get all actors that implement the interface
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, USaveInterface::StaticClass(), Actors);
	for (auto* Actor : Actors)
		SaveInterfaces.Add(Actor);
}

void USaveManager::SaveGame()
{
	// Create a new save game data instance
	USaveGameData* const SaveGameData = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
	
	QueryAllSaveInterfaces();
	SaveParameters(SaveGameData);

	// Saves the game to the current save slot
	UGameplayStatics::SaveGameToSlot(SaveGameData, CurrentSaveSlot, 0);

	// Update the save's metadata
	USaveGameMetadata* const SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));
	auto& [SlotName, DateTime, LevelToSave] = SaveGameMetadata->SavedGamesMetadata.FindOrAdd(CurrentSaveSlot);
	SlotName = CurrentSaveSlot;
	DateTime = FDateTime::Now();
	LevelToSave = UGameplayStatics::GetCurrentLevelName(GameInstance->GetWorld(), true);

	UGameplayStatics::SaveGameToSlot(SaveGameMetadata, MetadataSaveSlot, 0);

	if (GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Saved: " + CurrentSaveSlot);
}

void USaveManager::SaveParameters(USaveGameData* const SaveGameData)
{
	for (auto& SaveInterface : SaveInterfaces)
	{
		if (SaveInterface.GetObject() == nullptr)
			continue;

		// Let Object know its about to be saved
		SaveInterface->Execute_OnBeforeSave(SaveInterface.GetObject());

		// Find the object's save data, using it's unique name
		const FString UniqueSaveName = SaveInterface->Execute_GetUniqueSaveName(SaveInterface.GetObject());
		auto& [Data] = SaveGameData->SerialisedData.Add(UniqueSaveName);
		
		FMemoryWriter MemoryWriter = FMemoryWriter(Data);
		MemoryWriter.ArIsSaveGame = true;

		SaveInterface.GetObject()->Serialize(MemoryWriter);
	}
}

void USaveManager::LoadSavedLevel()
{
	const USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));
	const FString LevelToOpen = SaveGameMetadata->SavedGamesMetadata.Find(CurrentSaveSlot)->SavedLevelName;
	GameInstance->HostGame("Test", LevelToOpen);
}

void USaveManager::LoadGame()
{
	const USaveGameData* SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0));
	if (SaveGameData == nullptr)
	{
		SaveGame();		// No Saves exist for this slot, so create a default one
		SaveGameData = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlot, 0)); // Reload it
	}
	LoadSavedParameters(SaveGameData);
	
	if (GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "Loaded: " + CurrentSaveSlot);
}

void USaveManager::LoadSavedParameters(const USaveGameData* const SaveGameData)
{
	for (auto& SaveInterface : SaveInterfaces)
	{
		if (SaveInterface.GetObject() == nullptr)
			continue;

		// Find the object's save data, using it's unique name
		const FString UniqueSaveName = SaveInterface->Execute_GetUniqueSaveName(SaveInterface.GetObject());
		const FSaveData* SaveData = SaveGameData->SerialisedData.Find(UniqueSaveName);
		
		// if (SaveData == nullptr)
		// 	continue;
		
		FMemoryReader MemoryReader(SaveData->Data);
		MemoryReader.ArIsSaveGame = false;
		// Essentially load the data from the save
		SaveInterface.GetObject()->Serialize(MemoryReader);
	}
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
	const USaveGameMetadata* SaveGameMetadata = Cast<USaveGameMetadata>(UGameplayStatics::LoadGameFromSlot(MetadataSaveSlot, 0));

	// Allocate exact memory for array  
	Metadata.Reserve(SaveGameMetadata->SavedGamesMetadata.Num());
	// Add each save game's Metadata to the return array
	for (const auto& SavedGame : SaveGameMetadata->SavedGamesMetadata)
	{
		Metadata.Push(SavedGame.Value);
	}
	
	Metadata.Sort([](const FSaveMetadata& SaveA, const FSaveMetadata& SaveB) {
		return  SaveA.DateTime > SaveB.DateTime;
	});
	
	return Metadata;
}
