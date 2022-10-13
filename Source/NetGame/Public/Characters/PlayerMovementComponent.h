// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

/**
 * Character component, derived from the default character movement
 * component to handle replicating different movement abilities
 */
UCLASS()
class NETGAME_API UPlayerMovementComponent final : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FPlayer_SavedMove_Character;
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Variables")
	float WalkSpeed;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Variables")
	float RunSpeed;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Variables")
	float AimSpeed;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Variables")
	float DodgeStrength;

	/** Set the Sprinting action */
	UFUNCTION(BlueprintCallable)
	void SetSprinting(bool Sprint);
	/** Trigger the Dodge action */
	UFUNCTION(BlueprintCallable)
	void SetAim(bool Aim);
	/** Trigger the Dodge action */
	UFUNCTION(BlueprintCallable)
	void DoDodge();

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerSetMoveDirection(const FVector& MoveDir);

private:
	uint8 bWantsToSprint : 1;
	uint8 bWantsToDodge : 1;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	uint8 bWantsToAim : 1;
	FVector MoveDirection;
	
public:
	UPlayerMovementComponent();
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
};

class FPlayer_SavedMove_Character final : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;
	/// @brief Reset all stored variables
	virtual void Clear() override;
	/// @brief Store input commands in compressed flags
	virtual uint8 GetCompressedFlags() const override;
	/// @brief Sets up the move before sending it to the server.
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	/// @brief Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;

private:
	uint8 bSavedWantsToSprint : 1;
	FVector SavedMoveDirection;
	uint8 bSavedWantsToDodge : 1;
	uint8 bSavedWantsToAim : 1;
};

class FPlayer_NetworkPredictionData_Client_Character final : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;
	FPlayer_NetworkPredictionData_Client_Character(const UCharacterMovementComponent* CharacterMovementComponent)
	 : Super(*CharacterMovementComponent) {}
	virtual FSavedMovePtr AllocateNewMove() override;
};
