#pragma once

#include "Runtime/CoreUObject/Public/UObject/Class.h"
#include "SaveMetadata.generated.h"

USTRUCT(BlueprintType)
struct FSaveMetadata
{
	GENERATED_BODY()

	/// @brief Name of the Slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Save & Load")
	FString SlotName;
	
	/// @brief Slot's date of creation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Save & Load")
	FDateTime DateTime;
};
