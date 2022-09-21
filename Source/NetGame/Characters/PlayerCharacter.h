// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AbilitySystemInterface.h"
#include "NetGame/GAS/GASAbilitySystemComponent.h"
#include "NetGame/GAS/BaseGameplayAbility.h"
#include "NetGame/SaveLoad/SaveInterface.h"

#include "PlayerCharacter.generated.h"

class UPlayerMovementComponent;
UCLASS()
class NETGAME_API APlayerCharacter : public ACharacter, public ISaveInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category="PlayerMovementComponent")
	UPlayerMovementComponent* GetPlayerMovementComponent() const;
	
	void MoveForward(float InputAxisValue);
	void MoveRight(float InputAxisValue);

	/**
	* @brief Whether the targeted PlayerCharacter is controlled by the player on the server
	*
	* Perhaps there is no need to check, since the Host will have loaded
	* a save slot which will be different than the default _Development one,
	* hence the client will not access the server's data;
	* Except if there is a _Develpment slot leftover from PIE testing sessions
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Networking")
	bool IsHostCharacter() const;
	
// Saving system
protected:
	virtual void OnBeforeSave_Implementation() override;
	void LoadSavedParameters();
	
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Saved Variables")
	FTransform PlayerTransform;

// GameplayAbilities system
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="GAS")
	UGASAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="GAS")
	class UPlayerAttributeSet* AttributeSet;
	
	/** Effect to initialize the attributes' default values */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffectsToApply;
	  
	/** Default abilities to be given at the beginning */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="GAS")
	TArray<TSubclassOf<UBaseGameplayAbility>> DefaultAbilities;
	
	/** Called on both server and client */
	virtual void InitializeAttributes();
	/** Called only on server */
	virtual void GiveDefaultAbilities();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Called both on OnRep_PlayerState and SetupPlayerInput */
	void BindASCInput();

private:
	bool bIsASCInputBound = false;
	FVector2D InputVector;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector2D GetInputVector() const { return InputVector; }
};
