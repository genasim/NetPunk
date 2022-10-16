// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "SaveLoad/SaveManager.h"


UEOSGameInstance::UEOSGameInstance()
{
	bIsLoggedIn = false;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	USaveManager::Init(this);
	
	OnlineSubsystem = IOnlineSubsystem::Get();
	SessionPtr = OnlineSubsystem->GetSessionInterface();
	Identity = OnlineSubsystem->GetIdentityInterface();
	
	LoginEOS();
}

void UEOSGameInstance::LoginEOS()
{
	FOnlineAccountCredentials Credentials;
	Credentials.Type = FString("developer");		//	developer type insists that the DevAuth tool is running in the background
	Credentials.Id = FString("localhost:6000");
	Credentials.Token = FString("Devtester");
	
	Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
	Identity->Login(0, Credentials);
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Login Delegate fired -> Success: %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;
	if (!bWasSuccessful)
	{
		ShowErrorMessageEOS.Broadcast(TEXT("Login Failed!"));
		return;
	}
	
	Identity->ClearOnLoginCompleteDelegates(0, this);
}

void UEOSGameInstance::CreateSession()
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.Set(SEARCH_KEYWORDS, FString("GameLobby"), EOnlineDataAdvertisementType::ViaOnlineService);
		
	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
	SessionPtr->CreateSession(0, DefaultSessionName, SessionSettings);
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("CreateSession Delegate fired -> Success: %d"), Succeeded)
	if (!bIsLoggedIn) {
		ShowErrorMessageEOS.Broadcast(TEXT("User not logged in!"));
		return;
	} if (!Succeeded) {
		ShowErrorMessageEOS.Broadcast(TEXT("Could not create session!"));
		return;
	}

	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
}

