// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NETGAME_API ISaveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/// @brief Get a unique name to use when saving
	UFUNCTION(BlueprintNativeEvent, Category="Save & Load | SaveInterface")
	FString GetUniqueSaveName();

	/// @brief Called right before the owning actor is saved
	UFUNCTION(BlueprintNativeEvent, Category="Save & Load | SaveInterface")
	void OnBeforeSave(); 
};
