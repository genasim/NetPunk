// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NetGame/NetGame.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class NETGAME_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBaseGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Abilities")
	EGASAbilityInputID AbilityInputID = EGASAbilityInputID::None;
};
