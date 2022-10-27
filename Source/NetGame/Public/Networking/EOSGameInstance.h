// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Characters/PlayerCharacter.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowErrorMessageEOS, FString, ErrorMessage);

struct FSessionInfo
{
public:
	FUniqueNetIdPtr UserId;
	int32 LocalUserNum;
	FOnlineSessionSearchResult SessionResult;
	FName SessionName;
};

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
	void DestroySession();
	UFUNCTION(BlueprintCallable, Category="EOS Networking")
	void ShowInviteFriendsUI();

	void CreateSession();
	void JoinGame(const FSessionInfo* Session);
	
protected:
	IOnlineSessionPtr SessionPtr;
	IOnlineIdentityPtr IdentityPtr;
	IOnlineExternalUIPtr ExternalUIPtr;
	
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	
	void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful,
		const FUniqueNetId& UserId, const FString& Error);
	void OnDestroySessionComplete(FName SessionName, bool Succeeded);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
		FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="EOS Networking")
	FShowErrorMessageEOS ShowErrorMessageEOS;

private:
	bool EnsureOnlinePointersValidity();
	FSessionInfo SessionInfo;
	
	FString LevelToOpen = "TutorialCave";
	bool bIsLoggedIn;
	
public:
	/// @brief Returns the APlayerCharacter for this UNWGameInstance
	UFUNCTION(BlueprintPure, Category="Networking")
	static APlayerCharacter* GetLocalPlayerCharacter();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Networking")
	static void ServerTravelBP(const FString& LevelAddress, const bool Absolute, const bool ShouldSkipGameNotify);

	UFUNCTION(BlueprintCallable, Category="Networking")
	void ServerTravel(const FString& LevelAddress);
};
