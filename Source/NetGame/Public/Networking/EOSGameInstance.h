// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Characters/PlayerCharacter.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly, Category="EOS Networking")
	void CreateSession();
	UFUNCTION(BlueprintCallable, Category="EOS Networking")
	void DestroySession();
	void JoinGame(FUniqueNetIdPtr UserID, const FOnlineSessionSearchResult& SessionToJoin);
	UFUNCTION(BlueprintCallable, Category="EOS Networking")
	void ShowInviteFriendsUI();
	
protected:
	void FetchOnlinePointers();
	IOnlineSessionPtr SessionPtr;
	IOnlineIdentityPtr Identity;
	IOnlineExternalUIPtr ExternalUIPtr;
	

	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful,
		const FUniqueNetId& UserId, const FString& Error);
	virtual void OnDestroySessionComplete(FName SessionName, bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
		FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="EOS Networking")
	FShowErrorMessageEOS ShowErrorMessageEOS;

private:
	FName DefaultSessionName = "Test Session";
	FString LevelToOpen = "TutorialCave";
	bool bIsLoggedIn;

public:
	/// @brief Returns the APlayerCharacter for this UNWGameInstance
	UFUNCTION(BlueprintPure, Category="Networking")
	static APlayerCharacter* GetLocalPlayerCharacter();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Networking")
	static void ServerTravelBP(const FString& LevelAddress, const bool Absolute, const bool ShouldSkipGameNotify);
};
