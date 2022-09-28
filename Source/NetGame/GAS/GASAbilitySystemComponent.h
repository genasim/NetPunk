// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseGameplayAbility.h"
#include "PlayerAttributeSet.h"
#include "GASAbilitySystemComponent.generated.h"

UENUM()
enum EAttributeTableValue
{
	Base,
	Max,
	Min
};

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
	
	UPROPERTY()
	const UPlayerAttributeSet* AttributeSet;

public:
	UGASAbilitySystemComponent();
	
	/** Called on both server and client */
	void ApplyDefaultEffects();
	/** Called only on server */
	void GiveDefaultAbilities();

	virtual void BeginPlay() override;
	
	/**
	 * Methods for accessing the Default Attribute DataTable
	 * Not sure how to tie them to the AttributeSet's methods
	 *		(or how to use them or even do so)
	 */
private:
	UPROPERTY()
	UDataTable* AttributesDataTable;
	
	float GetHealthFromTable(const EAttributeTableValue Column) const;
	float GetStaminaFromTable(const EAttributeTableValue Column) const;
	float GetHealthRegenFromTable(const EAttributeTableValue Column) const;
	float GetStaminaRegenFromTable(const EAttributeTableValue Column) const;
};

