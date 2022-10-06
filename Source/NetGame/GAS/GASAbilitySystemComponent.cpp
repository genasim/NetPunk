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
	if (!DefaultAbilities.Num())
		return;
	
	for (auto& StartupAbility : DefaultAbilities)
	{
		GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));	
	}
}
