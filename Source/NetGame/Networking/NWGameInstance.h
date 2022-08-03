// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NWGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETGAME_API UNWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UNWGameInstance();

protected:
	virtual void Init() override;
	IOnlineSessionPtr SessionInterface;

virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);

	UFUNCTION(BlueprintCallable)
	void HostGame();
	UFUNCTION(BlueprintCallable)
	void JoinGame();
};
