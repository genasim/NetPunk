// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameData.h"
#include "SaveInterface.h"
#include "SaveMetadata.h"
#include "Networking/EOSGameInstance.h"
// #include "Networking/NWGameInstance.h"
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
	static void Init(UEOSGameInstance* InGameInstance);

	/**
	 *	@brief Queries all the actors in the game that implement the save interface
	 *	and add them to the array
	**/
	static void QueryAllSaveInterfaces();
	static void ClearSaveInterfaceArray() { SaveInterfaces.Empty(); }
	
	/**
	*	Saves the current state of the game to the CurrentSaveSlot.
	*	@note Will also call SaveParameters
	*/
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void SaveGame();

	/**
	*	Load a saved instance of the game from the current save slot.
	*	Will create a new save if CurrentSaveSlot is empty
	*	@note Will also call LoadSavedParameters
	*/
	static void LoadGame();
	/** Load the saved level from the save metadata */
	UFUNCTION(BlueprintCallable, Category="Save & Load")
	static void LoadSavedLevel();

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

	/// @brief Get all save games sorted by DateTime
	UFUNCTION(BlueprintPure, Category="Save & Load")
	static TArray<FSaveMetadata> GetAllSaveMetadata();

private:
	///	@brief Loop over all actors that need to load data and do it
	static void LoadSavedParameters(const USaveGameData* const SaveGameData);
	/// @brief Loop over all actors in the SaveInterfaces array that need to be saved and save them
	static void SaveParameters(USaveGameData* const SaveGameData);
	
	/// @brief All actors in the game that implement the save interface
	static TArray<TScriptInterface<ISaveInterface>> SaveInterfaces;

	static FString CurrentSaveSlot;
	// static UNWGameInstance* GameInstance;
	static UEOSGameInstance* GameInstance;
	
	inline static const FString MetadataSaveSlot = "SaveGameMetadata";
	inline static constexpr int32 MaxSaveSlots = 15;
};
