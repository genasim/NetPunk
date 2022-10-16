// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"	// Used for IOnlineSessionPtr
#include "EOSGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowErrorMessageEOS, FString, ErrorMessage);

/**
 * 
 */
UCLASS()
class NETGAME_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOSGameInstance();
	virtual void Init() override;

	void LoginEOS();

	UFUNCTION(BlueprintCallable, Category="EOS Networking")
	void CreateSession();

protected:
	IOnlineSubsystem* OnlineSubsystem;
	IOnlineSessionPtr SessionPtr;
	IOnlineIdentityPtr Identity;
	
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="EOS Networking")
	FShowErrorMessageEOS ShowErrorMessageEOS;

private:
	FName DefaultSessionName = "Test Session";
	FString LevelToOpen = "TutorialCave";

	bool bIsLoggedIn;
};
