// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineFriendsInterface.h"
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
	Credentials.Type = FString();		//	developer type insists that the DevAuth tool is running in the background
	Credentials.Id = FString();			//	Since we are now using EOSPlus we login through Steam
	Credentials.Token = FString();
	
	Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
	Identity->Login(0, Credentials);
}

void UEOSGameInstance::GetAllFriends()
{
	const IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface();
	FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UEOSGameInstance::OnGetAllFriendsComplete));
	
}

void UEOSGameInstance::OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName,
	const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Friendslist -> Success: %d"), bWasSuccessful)
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                       const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Login Delegate fired -> Success: %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;
	if (!bWasSuccessful)
		ShowErrorMessageEOS.Broadcast(Error);
	
	Identity->ClearOnLoginCompleteDelegates(0, this);
}

void UEOSGameInstance::CreateSession(const FString OpenLevelName)
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

	if (OpenLevelName != "")
		LevelToOpen = OpenLevelName;	//	Otherwise use the Default Level -> meant for when session is created through the NewGame menu
	
	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
	SessionPtr->CreateSession(0, DefaultSessionName, SessionSettings);
	GetWorld()->ServerTravel("/Game/Levels/" + LevelToOpen + "?listen");
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

void UEOSGameInstance::DestroySession()
{
	SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
	SessionPtr->DestroySession(DefaultSessionName);
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	USaveManager::ClearSaveInterfaceArray();
	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
}

