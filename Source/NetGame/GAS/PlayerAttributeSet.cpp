// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"

#include "Net/UnrealNetwork.h"


UPlayerAttributeSet::UPlayerAttributeSet() {}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, OldMaxHealth);
}

void UPlayerAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldStamina);
}

void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, OldMaxStamina);
}

void UPlayerAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UPlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MoveSpeed, OldMoveSpeed);
}
