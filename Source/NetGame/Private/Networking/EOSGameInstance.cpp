// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/SaveManager.h"


UEOSGameInstance::UEOSGameInstance()
{
	bIsLoggedIn = false;
}

bool UEOSGameInstance::EnsureOnlinePointersValidity()
{
	if (SessionPtr && IdentityPtr && ExternalUIPtr)
		return true;

	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
		return false;

	SessionPtr = OnlineSubsystem->GetSessionInterface();
	IdentityPtr = OnlineSubsystem->GetIdentityInterface();
	ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();

	return true;
}

void UEOSGameInstance::Init()
{
	Super::Init();

	USaveManager::Init(this);
	
	if (EnsureOnlinePointersValidity())
	{
		IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &ThisClass::OnSessionUserInviteAccepted);
		SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnDestroySessionComplete);
		SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
	}
	
	LoginEOS();
}

void UEOSGameInstance::LoginEOS()
{
	FOnlineAccountCredentials Credentials;
	Credentials.Type = FString("");		//	developer type insists that the DevAuth tool is running in the background
	Credentials.Id = FString("");		//	Since we are now using EOSPlus we login through Steam and don't need DevAuth
	Credentials.Token = FString("");

	if (EnsureOnlinePointersValidity())
		IdentityPtr->Login(0, Credentials);
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	IdentityPtr->ClearOnLoginCompleteDelegates(0, this);
	
	UE_LOG(LogTemp, Warning, TEXT("Login Delegate fired -> Success: %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;
	if (!bWasSuccessful)
		ShowErrorMessageEOS.Broadcast(Error);

	DefaultSessionName = FName(*UserId.ToString());
	CreateSession();
}

void UEOSGameInstance::CreateSession()
{
	if (!bIsLoggedIn)
		return;
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = false;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.NumPrivateConnections = 2;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.Set(SEARCH_KEYWORDS, FString("GameLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

	if (EnsureOnlinePointersValidity())
		SessionPtr->CreateSession(0, DefaultSessionName, SessionSettings);
}

void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	SessionPtr->ClearOnCreateSessionCompleteDelegates(this);

	UE_LOG(LogTemp, Warning, TEXT("CreateSession Delegate fired -> Success: %d"), Succeeded)
	if (!bIsLoggedIn) {
		ShowErrorMessageEOS.Broadcast(TEXT("User not logged in!"));
		return;
	} if (!Succeeded) {
		ShowErrorMessageEOS.Broadcast(TEXT("Could not create session!"));
		return;
	}
}

void UEOSGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
	const FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	SessionPtr->ClearOnSessionUserInviteAcceptedDelegates(this);
	
	UE_LOG(LogTemp, Warning, TEXT("Session User Invite Accepted -> Success: %d"), bWasSuccessful)
	// if (!bWasSuccessful)
		// return;
	
	// ensureMsgf(InviteResult.IsValid(), TEXT("Invite Result is Invalid!!"));
	// if (!InviteResult.IsValid())
	// 	return;
	//
	// ShowErrorMessageEOS.Broadcast("Client has joined game");
	// JoinGame(UserId, InviteResult);
	UE_LOG(LogTemp, Warning, TEXT("OnSessionInviteAccepted LocalUserNum: %d bSuccess: %d"), LocalUserNum, bWasSuccessful);
	DestroySession();
	if (bWasSuccessful)
	{
		if (InviteResult.IsValid())
		{
			IOnlineSessionPtr SessionInt = IOnlineSubsystem::Get()->GetSessionInterface();
			SessionInt->JoinSession(LocalUserNum, DefaultSessionName, InviteResult);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Invite accept returned no search result."));
	}
}

void UEOSGameInstance::JoinGame(const FUniqueNetIdPtr UserID, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (!SessionToJoin.IsValid())
		return;

	if (EnsureOnlinePointersValidity())
		SessionPtr->JoinSession(UserID->AsShared().Get(), DefaultSessionName, SessionToJoin);
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Delegate -> Success: %d"), (Result == EOnJoinSessionCompleteResult::Success) ? true : false);
	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);
	if (Result != EOnJoinSessionCompleteResult::Success)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("JoinSessionCompleted"));
	if (EnsureOnlinePointersValidity())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sessions Valid"));
		// Client travel to the server
		FString ConnectString;
		if (!SessionPtr->GetResolvedConnectString(SessionName, ConnectString))
		{
			UE_LOG(LogTemp, Error, TEXT("ConnectionInfo is Empty!!"))
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Join session: traveling to %s"), *ConnectString);
		GetFirstLocalPlayerController()->ClientTravel(ConnectString, TRAVEL_Absolute);
	}
}

void UEOSGameInstance::DestroySession()
{
	USaveManager::ClearSaveInterfaceArray();
	
	if (EnsureOnlinePointersValidity())
		SessionPtr->DestroySession(DefaultSessionName);
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
	UE_LOG(LogTemp, Warning, TEXT("Destroy session Delegate -> Success: %d"), Succeeded)
}

void UEOSGameInstance::ShowInviteFriendsUI()
{
	if (EnsureOnlinePointersValidity())
	{
		if (!bIsLoggedIn)
			return;
		ExternalUIPtr->ShowInviteUI(0, DefaultSessionName);
	}
}

///////////////////////////////////////
///		Helper functions			///

APlayerCharacter* UEOSGameInstance::GetLocalPlayerCharacter()
{
	if (GEngine != nullptr)
		return Cast<APlayerCharacter>(UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0)->GetPawn());
	return nullptr;
}

void UEOSGameInstance::ServerTravelBP(const FString& LevelAddress, const bool Absolute, const bool ShouldSkipGameNotify)
{
	if (GEngine != nullptr)
		GEngine->GameViewport->GetWorld()->ServerTravel("/Game/Levels/" + LevelAddress + "?listen", Absolute, ShouldSkipGameNotify);
}