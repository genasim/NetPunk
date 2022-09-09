// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetGame/SaveLoad/SaveInterface.h"
#include "PlayerCharacter.generated.h"

class UPlayerMovementComponent;
UCLASS()
class NETGAME_API APlayerCharacter : public ACharacter, public ISaveInterface
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
	
// ISaveInterface
protected:
	virtual void OnBeforeSave_Implementation() override;
	void LoadSavedParameters();
	
	UPROPERTY(SaveGame, BlueprintReadOnly, Category="Saved Variables")
	FTransform PlayerTransform;
};
