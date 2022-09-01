// Fill out your copyright notice in the Description page of Project Settings.


#include "NWGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

UNWGameInstance::UNWGameInstance()
{
	DefaultSessionName = "My Session Name";
}

void UNWGameInstance::Init()
{
	Super::Init();
	if (!IOnlineSubsystem::Get()) return;

	SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNWGameInstance::OnCreateSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNWGameInstance::OnFindSessionComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNWGameInstance::OnJoinSessionComplete);
		SessionInterface->OnSessionFailureDelegates.AddUObject(this, &UNWGameInstance::OnSessionFailure);
	}
}

void UNWGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Delegate fired -> Success: %d"), Succeeded)
	if (!Succeeded)
		return;

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController);
	GetWorld()->ServerTravel("/Game/Levels/TutorialCave?listen");
}

void UNWGameInstance::OnFindSessionComplete(bool Succeeded)
{
	IsSearchingServersDelegate.Broadcast(false);
	
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Delegate fired -> Success: %d"), Succeeded)
	if (!Succeeded)
		return;
	
	const TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		
	UE_LOG(LogTemp, Warning, TEXT("Sessions found -> %d"), SearchResults.Num())
	if (!SearchResults.Num())
	{
		ShowErrorMessage.Broadcast(TEXT("No sessions Found!"));
		return;
	}

	if (bQuickSearch)
	{
		bQuickSearch = false;
		const auto Index = FMath::RandRange(0,SearchResults.Num()-1);
		SessionInterface->JoinSession(0, DefaultSessionName, SearchResults[Index]);	
		return;
	}
		
	int8 ArrayIndex = -1;
	for (auto Result : SearchResults)
	{
		if (!Result.IsValid())
			continue;
			
		FServerInfo ServerInfo;
		FString ServerName = "Empty ServerName";
		FString HostName = "Empty HostName";

		Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);
		Result.Session.SessionSettings.Get(FName("SERVER_HOSTNAME_KEY"), HostName);
		
		ArrayIndex++;
		ServerInfo.ServerName = ServerName;
		ServerInfo.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		ServerInfo.CurrentPlayers = ServerInfo.MaxPlayers - Result.Session.NumOpenPublicConnections;
		ServerInfo.SetPlayerCount();
		ServerInfo.PingInMs = Result.PingInMs;
		ServerInfo.isLan = Result.Session.SessionSettings.bIsLANMatch;
		ServerInfo.ServerArrayIndex = ArrayIndex;

		UE_LOG(LogTemp, Warning, TEXT("Server Name: %s | Server Index: %d"), *ServerInfo.ServerName, ArrayIndex)
		AddServerSlotDelegate.Broadcast(ServerInfo);
	}
}

void UNWGameInstance::OnJoinSessionComplete(FName ServerName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete Delegate fired: ServerName -> %s"), *ServerName.ToString())
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(ServerName, JoinAddress);
		if (JoinAddress == "")
			return;
		
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController);
		PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UNWGameInstance::OnSessionFailure(const FUniqueNetId& NetID, ESessionFailure::Type)
{
	// if ()
}

void UNWGameInstance::HostGame(FCreateServerInfo ServerInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session"))
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.NumPublicConnections = ServerInfo.MaxPlayers;

	// TODO: Set to use ServerInfo.IsLan
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") ? true : false;

	SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerInfo.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	// SessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, DefaultSessionName, SessionSettings);
}

void UNWGameInstance::SearchServers()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching sessions ..."))
	IsSearchingServersDelegate.Broadcast(true);
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() != "NULL" ? false : true;	

	bQuickSearch = false;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNWGameInstance::CancelSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Session search canceled"))
	IsSearchingServersDelegate.Broadcast(false);
	SessionInterface->CancelFindSessions();
}

void UNWGameInstance::QuickJoin()
{
	UE_LOG(LogTemp, Warning, TEXT("Quick search ... "))
	IsSearchingServersDelegate.Broadcast(true);
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() != "NULL" ? false : true;
	
	bQuickSearch = true;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNWGameInstance::JoinServer(int32 ArrayIndex)
{
	const auto Result = SessionSearch->SearchResults[ArrayIndex];

	if (!Result.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER AT INDEX %d"), ArrayIndex);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("JOINING SERVER AT INDEX %d"), ArrayIndex);
	SessionInterface->JoinSession(0, DefaultSessionName, Result);	
}
