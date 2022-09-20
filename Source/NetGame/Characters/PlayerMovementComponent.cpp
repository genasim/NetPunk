// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent.h"

#include "GameFramework/Character.h"



UPlayerMovementComponent::UPlayerMovementComponent() {}

void UPlayerMovementComponent::SetSprinting(const bool Sprint) 
{
	bWantsToSprint = Sprint;
}

void UPlayerMovementComponent::ServerSetMoveDirection_Implementation(const FVector& MoveDir)
{
	MoveDirection = MoveDir;
}

bool UPlayerMovementComponent::ServerSetMoveDirection_Validate(const FVector& MoveDir)
{
	return true;
}

void UPlayerMovementComponent::DoDodge()
{
	bWantsToDodge = true;
}

void UPlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (!CharacterOwner) return;
	
	if (PawnOwner->IsLocallyControlled())
	{
		MoveDirection = GetLastInputVector() != FVector::ZeroVector ?	PawnOwner->GetLastMovementInputVector() :
																		PawnOwner->GetActorForwardVector() * -1;
		if (PawnOwner->GetLocalRole() < ROLE_Authority)
			ServerSetMoveDirection(MoveDirection);
	}

	if (!IsMovingOnGround())
		bWantsToDodge = false;
	else
		FallingLateralFriction = 0.25f;

	if (bWantsToDodge)
	{
		MoveDirection.Normalize();
		FVector DodgeVelocity = MoveDirection * DodgeStrength;
		DodgeVelocity.Z = 0.0f;
		FallingLateralFriction = 6.5f;
		Launch(DodgeVelocity);
	}
}

void UPlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToDodge = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UPlayerMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		if (UPlayerMovementComponent* TempComponent = const_cast<UPlayerMovementComponent*>(this))
			TempComponent->ClientPredictionData = new FPlayer_NetworkPredictionData_Client_Character(this);
	}
	
	return ClientPredictionData;
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
		case MOVE_Walking:
		case MOVE_NavWalking:
		{
			if (IsCrouching())
				return MaxWalkSpeedCrouched;
			if (bWantsToSprint)
				return RunSpeed;
			return WalkSpeed;
		}
		case MOVE_Falling:
			return WalkSpeed;
		case MOVE_Flying:
			return MaxFlySpeed;
		case MOVE_Swimming:
			return MaxSwimSpeed;
		default:
			return 0.0f;
	}
}


// class UPlayerMovementComponent
////////////////////////////////////////////////////////////
// class FPlayer_SavedMove_Character

void FPlayer_SavedMove_Character::Clear()
{
	Super::Clear();
	bSavedWantsToSprint = false;
	bSavedWantsToDodge = false;
	SavedMoveDirection = FVector::ZeroVector;
}

uint8 FPlayer_SavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToSprint)
		Result |= FLAG_Custom_0;
	if (bSavedWantsToDodge)
		Result |= FLAG_Custom_1;
	return Result;
}

void FPlayer_SavedMove_Character::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	if (!C) return;
	if (const UPlayerMovementComponent* PlayerMovementComponent = Cast<UPlayerMovementComponent>(C->GetCharacterMovement()))
	{
		// Storing data to be saved and sent across
		bSavedWantsToSprint = PlayerMovementComponent->bWantsToSprint;
		bSavedWantsToDodge = PlayerMovementComponent->bWantsToDodge;
		SavedMoveDirection = PlayerMovementComponent->MoveDirection;
	}
}

void FPlayer_SavedMove_Character::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	
	if (!C) return;
	if (UPlayerMovementComponent* PlayerMovementComponent = Cast<UPlayerMovementComponent>(C->GetCharacterMovement()))
	{
		PlayerMovementComponent->MoveDirection = SavedMoveDirection;
	}
}

bool FPlayer_SavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	float MaxDelta) const
{
	if (bSavedWantsToSprint != ((FPlayer_SavedMove_Character*)&NewMove)->bSavedWantsToSprint)
		return false;
	if (bSavedWantsToDodge != ((FPlayer_SavedMove_Character*)&NewMove)->bSavedWantsToDodge)
		return false;
	if (SavedMoveDirection != ((FPlayer_SavedMove_Character*)&NewMove)->SavedMoveDirection)
		return false;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

// class FPlayer_SavedMove_Character
////////////////////////////////////////////////////////////
// class FPlayer_NetworkPredictionData_Client_Character

FSavedMovePtr FPlayer_NetworkPredictionData_Client_Character::AllocateNewMove()
{
	return MakeShared<FPlayer_SavedMove_Character>();
}
