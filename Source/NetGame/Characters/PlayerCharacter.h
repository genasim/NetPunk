// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class NETGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveForward(float InputAxisValue);
	UFUNCTION()
	void MoveRight(float InputAxisValue);

	UFUNCTION(BlueprintCallable)
	void StartSprint();
	UFUNCTION(BlueprintCallable)
	void EndSprint();

	UPROPERTY(EditAnywhere, Category="Variables")
	float WalkingSpeed = 380.0f;
	UPROPERTY(EditAnywhere, Category="Variables")
	float RunningSpeed = 800.0f;
};
