// Fill out your copyright notice in the Description page of Project Settings.


#include "GASAbilitySystemComponent.h"


UGASAbilitySystemComponent::UGASAbilitySystemComponent()
{
	SetIsReplicated(true);
	UAbilitySystemComponent::SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void UGASAbilitySystemComponent::ApplyDefaultEffects()
{
	if (!DefaultEffectsToApply.Num())
		return;

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const auto Effect : DefaultEffectsToApply)
	{
		const FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(Effect, 1, EffectContext);
		if (!NewHandle.IsValid())
			return;

		ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this);
	}
}

void UGASAbilitySystemComponent::GiveDefaultAbilities()
{
	if (!GetOwner()->HasAuthority() || this == nullptr)
		return;
	
	for (auto& StartupAbility : DefaultAbilities)
	{
		GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));	
	}
}

void UGASAbilitySystemComponent::BeginPlay()
{
	AttributeSet = Cast<UPlayerAttributeSet>(GetAttributeSet(UPlayerAttributeSet::StaticClass()));
	AttributesDataTable = DefaultStartingData[0].DefaultStartingTable;
}

float UGASAbilitySystemComponent::GetHealthFromTable(const EAttributeTableValue Column) const
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

float UGASAbilitySystemComponent::GetStaminaFromTable(const EAttributeTableValue Column) const
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

float UGASAbilitySystemComponent::GetHealthRegenFromTable(const EAttributeTableValue Column) const
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

float UGASAbilitySystemComponent::GetStaminaRegenFromTable(const EAttributeTableValue Column) const
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
