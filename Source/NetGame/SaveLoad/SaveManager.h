// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveMetadata.h"
#include "UObject/NoExportTypes.h"
#include "SaveManager.generated.h"

/**
 * 
 */
UCLASS()
class NETGAME_API USaveManager : public UObject
{
	GENERATED_BODY()
	
public:
	/// @brief Initialise the class. Must be called when game first launches
	static void Init();

	/// @brief Queries all the actors in the game that implement the save interface
	static void QueryAllSaveInterfaces();

	/// @brief Saves the current state of the game
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void SaveGame();

	/// @brief Loads the saved current state of the game
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void LoadGame();

	/// @brief Delete the specified slot
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void DeleteSlot(const FString& SlotName);

	/// @brief Gets a new unused slot
	UFUNCTION(BlueprintPure, Category="Save & Load")
	static UPARAM(DisplayName="Slot") FString GetNewSaveSlot(bool& bSlotFound);
	
	/// @brief Sets the current save slot
	UFUNCTION(BlueprintPure, Category="Save & Load")
	static FString GetCurrentSaveSlot();

	/// @brief Gets the current save slot
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void SetCurrentSaveSlot(const FString& SlotName);

	/// @brief Get all save games
	UFUNCTION(BlueprintPure, Category="Save & Load")
	static TArray<FSaveMetadata> GetAllSaveMetaData();

private:
	static FString CurrentSaveSlot;
	inline static const FString MetaDataSaveSlot = "SaveGameMetaData";
	static const int32 MaxServerSlots = 15;

};
