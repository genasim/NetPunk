// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class NETGAME_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	enum EAttributeTableValue
	{
		Base,
		Max,
		Min
	};
	
public:
	UPlayerAttributeSet();

	virtual void InitFromMetaDataTable(const UDataTable* DataTable) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)

	// Health regen rate will passively increase Health every second
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, HealthRegenRate)

	// Current stamina, used to execute special abilities. Capped by MaxStamina.
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina)

	// Stamina regen rate will passively increase Stamina every second
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegenRate)
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, StaminaRegenRate)

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that exists only on the server. Doesn't replicate
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "AttackPower")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackPower)

	// MoveSpeed affects how fast characters can move.
	// Unused as of now
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MoveSpeed)

	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

private:	
	UPROPERTY()
	UDataTable* AttributesDataTable;
	
	float GetHealthFromTable(const EAttributeTableValue Column) const;
	float GetStaminaFromTable(const EAttributeTableValue Column) const;
	float GetHealthRegenRateFromTable(const EAttributeTableValue Column) const;
	float GetStaminaRegenRateFromTable(const EAttributeTableValue Column) const;
};
