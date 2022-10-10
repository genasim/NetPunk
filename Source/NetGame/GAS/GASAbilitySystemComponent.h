// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayAbility.h"
#include "GASAbilitySystemComponent.generated.h"


/**
 * 
 */
UCLASS()
class NETGAME_API UGASAbilitySystemComponent final : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	/** Effect to initialize the attributes' default values */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Defaults")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffectsToApply;
	  
	/** Default abilities to be given at the beginning */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Defaults")
	TArray<TSubclassOf<UBaseGameplayAbility>> DefaultAbilities;
	
public:
	UGASAbilitySystemComponent();
	
	/** Called on both server and client */
	void ApplyDefaultEffects();
	/** Called only on server */
	void GiveDefaultAbilities();
};
