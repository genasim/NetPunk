// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NWGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	// UPROPERTY(BlueprintReadOnly)
	// FString PlayerCount;
	UPROPERTY(BlueprintReadOnly)
	int CurrentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int MaxPlayers;
	UPROPERTY(BlueprintReadOnly)
	int PingInMs;
	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;
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

	UPROPERTY(BlueprintAssignable)
	FAddServerSlotDelegate AddServerSlotDelegate;
	UPROPERTY(BlueprintAssignable)
	FIsSearchingServersDelegate IsSearchingServersDelegate;
	UPROPERTY(BlueprintAssignable)
	FShowErrorMessage ShowErrorMessage;
	
	UFUNCTION(BlueprintCallable)
	void HostGame(FString ServerName, FString HostName);
	UFUNCTION(BlueprintCallable)
	void SearchServers();
	UFUNCTION(BlueprintCallable)
	void CancelSearch();
	UFUNCTION(BlueprintCallable)
	void QuickJoin();
	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

private:
	bool bQuickSearch = false;
	FName DefaultSessionName;
};
