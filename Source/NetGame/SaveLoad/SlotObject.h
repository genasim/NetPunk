// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveMetadata.h"
#include "UObject/NoExportTypes.h"
#include "SlotObject.generated.h"

class USlotObject; // todo: remove if problems occur
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotDeleted, USlotObject*, SlotToDelete);

/**
 * 
 */
UCLASS(BlueprintType)
class NETGAME_API USlotObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Save & Load | SaveSlot_UI object")
	FOnSlotDeleted OnSlotDeleted;
	
	UPROPERTY(BlueprintReadOnly, Category="Save & Load | SaveSlot_UI object", meta=(ExposeOnSpawn="true"))
	FSaveMetadata SaveMetadata = {};
};


