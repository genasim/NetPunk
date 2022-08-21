// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent.h"

void UPlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

FNetworkPredictionData_Client* UPlayerMovementComponent::GetPredictionData_Client() const
{
	return Super::GetPredictionData_Client();
}

// class UPlayerMovementComponent
////////////////////////////////////////////////////////////
// class FPlayer_SavedMove_Character

void FPlayer_SavedMove_Character::Clear()
{
	Super::Clear();
}

uint8 FPlayer_SavedMove_Character::GetCompressedFlags() const
{
	return Super::GetCompressedFlags();
}

void FPlayer_SavedMove_Character::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
}

void FPlayer_SavedMove_Character::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
}

bool FPlayer_SavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	float MaxDelta) const
{
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

// class FPlayer_SavedMove_Character
////////////////////////////////////////////////////////////
// class FPlayer_NetworkPredictionData_Client_Character

FSavedMovePtr FPlayer_NetworkPredictionData_Client_Character::AllocateNewMove()
{
	return Super::AllocateNewMove();
}
