// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UPlayerAttributeSet::UPlayerAttributeSet() {}

void UPlayerAttributeSet::InitFromMetaDataTable(const UDataTable* DataTable)
{
	Super::InitFromMetaDataTable(DataTable);
	AttributesDataTable = GetOwningAbilitySystemComponent()->DefaultStartingData[0].DefaultStartingTable;
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Clamp attributes between Min-Max values from their respective UDataTable
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())	
		SetHealth(FMath::Clamp(GetHealth(), GetHealthFromTable(Min), GetHealthFromTable(Max)));
	if (Data.EvaluatedData.Attribute == GetHealthRegenRateAttribute())	
		SetHealthRegenRate(FMath::Clamp(GetHealthRegenRate(), GetHealthRegenRateFromTable(Min), GetHealthRegenRateFromTable(Max)));
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())	
		SetStamina(FMath::Clamp(GetStamina(), GetStaminaFromTable(Min), GetStaminaFromTable(Max)));
	if (Data.EvaluatedData.Attribute == GetStaminaRegenRateAttribute())	
		SetStaminaRegenRate(FMath::Clamp(GetStaminaRegenRate(), GetStaminaRegenRateFromTable(Min), GetStaminaRegenRateFromTable(Max)));
}


float UPlayerAttributeSet::GetHealthFromTable(const EAttributeTableValue Column) const
{
	const FName RowName = FName(TEXT("PlayerAttributeSet.Health"));
	const FString Context = FString(TEXT("Getting Health Attribute"));
	const auto Row = AttributesDataTable->FindRow<FAttributeMetaData>(RowName, Context, true);

	switch (Column) {
	case Base: return Row->BaseValue;
	case Max: return Row->MaxValue;
	case Min: return Row->MinValue;
	default: checkNoEntry();
	}
	return 0;
}

float UPlayerAttributeSet::GetStaminaFromTable(const EAttributeTableValue Column) const
{
	const FName RowName = FName(TEXT("PlayerAttributeSet.Stamina"));
	const FString Context = FString(TEXT("Getting Stamina Attribute"));
	const auto Row = AttributesDataTable->FindRow<FAttributeMetaData>(RowName, Context, true);

	switch (Column) {
	case Base: return Row->BaseValue;
	case Max: return Row->MaxValue;
	case Min: return Row->MinValue;
	default: checkNoEntry();
	}
	return 0;
}

float UPlayerAttributeSet::GetHealthRegenRateFromTable(const EAttributeTableValue Column) const
{
	const FName RowName = FName(TEXT("PlayerAttributeSet.HealthRegen"));
	const FString Context = FString(TEXT("Getting HealthRegen Attribute"));
	const auto Row = AttributesDataTable->FindRow<FAttributeMetaData>(RowName, Context, true);

	switch (Column) {
	case Base: return Row->BaseValue;
	case Max: return Row->MaxValue;
	case Min: return Row->MinValue;
	default: checkNoEntry();
	}
	return 0;
}

float UPlayerAttributeSet::GetStaminaRegenRateFromTable(const EAttributeTableValue Column) const
{
	const FName RowName = FName(TEXT("PlayerAttributeSet.StaminaRegen"));
	const FString Context = FString(TEXT("Getting StaminaRegen Attribute"));
	const auto Row = AttributesDataTable->FindRow<FAttributeMetaData>(RowName, Context, true);

	switch (Column) {
	case Base: return Row->BaseValue;
	case Max: return Row->MaxValue;
	case Min: return Row->MinValue;
	default: checkNoEntry();
	}
	return 0;
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldStamina);
}

void UPlayerAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UPlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MoveSpeed, OldMoveSpeed);
}
