// Fill out your copyright notice in the Description page of Project Settings.


#include "NWGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"

UNWGameInstance::UNWGameInstance()
{
	
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
	}
}

void UNWGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Delegate fired -> Success: %d"), Succeeded)
	if (Succeeded)
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}

void UNWGameInstance::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Delegate fired -> Success: %d"), Succeeded)
	if (Succeeded)
	{
		const TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Sessions found -> %d"), SearchResults.Num())
		if (SearchResults.Num())
			SessionInterface->JoinSession(0, "Session", SearchResults[0]);		
	}
}

void UNWGameInstance::OnJoinSessionComplete(FName ServerName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete Delegate fired: ServerName -> %s"), *ServerName.ToString())
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(ServerName, JoinAddress);
		if (JoinAddress != "")
			PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}	
}

void UNWGameInstance::HostGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session"))
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.NumPublicConnections = 3;
	
	SessionInterface->CreateSession(0, "Session", SessionSettings);
}

void UNWGameInstance::JoinGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching sessions ..."))
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
