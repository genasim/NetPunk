// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGame/Characters/PlayerCharacter.h"
#include "NWGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FCreateServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString ServerName;
	UPROPERTY(BlueprintReadWrite)
	int MaxPlayers;
	UPROPERTY(BlueprintReadWrite)
	bool IsLan;
};

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;
	UPROPERTY(BlueprintReadOnly)
	int CurrentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int MaxPlayers;
	UPROPERTY(BlueprintReadOnly)
	bool isLan;
	UPROPERTY(BlueprintReadOnly)
	int PingInMs;
	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;
	
	void SetPlayerCount()
	{
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + " / " + FString::FromInt(MaxPlayers));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddServerSlotDelegate, FServerInfo, ServerInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIsSearchingServersDelegate, bool, IsSearching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowErrorMessage, FString, ErrorMessage);

/**
 * Game Instance class holding source for hosting and joining sessions
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
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName ServerName, EOnJoinSessionCompleteResult::Type Result);

	UPROPERTY(BlueprintAssignable, Category="Networking")
	FAddServerSlotDelegate AddServerSlotDelegate;
	UPROPERTY(BlueprintAssignable, Category="Networking")
	FIsSearchingServersDelegate IsSearchingServersDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Networking")
	FShowErrorMessage ShowErrorMessage;
	
	UFUNCTION(BlueprintCallable, Category="Networking")
	void HostGame(FCreateServerInfo CreateServerInfo, FString OpenLevelName);
	UFUNCTION(BlueprintCallable, Category="Networking")
	void SearchServers();
	UFUNCTION(BlueprintCallable, Category="Networking")
	void CancelSearch();
	UFUNCTION(BlueprintCallable, Category="Networking")
	void QuickJoin();
	UFUNCTION(BlueprintCallable, Category="Networking")
	void JoinServer(int32 ArrayIndex);

private:
	bool bQuickSearch = false;
	FName DefaultSessionName;
	FString LevelToOpen = "TutorialCave";

public:
	/// @brief Returns the APlayerCharacter for this UNWGameInstance
	UFUNCTION(BlueprintPure, Category="Networking")
	static APlayerCharacter* GetLocalPlayerCharacter();

	UFUNCTION(Server, Reliable, BlueprintCallable, BlueprintAuthorityOnly, Category="Networking")
	void ServerTravelBP(const FString& LevelAddress, const bool Absolute, const bool ShouldSkipGameNotify);
};
