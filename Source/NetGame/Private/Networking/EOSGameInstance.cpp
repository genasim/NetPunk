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

void UEOSGameInstance::FetchOnlinePointers()
{
	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionPtr = OnlineSubsystem->GetSessionInterface();
		Identity = OnlineSubsystem->GetIdentityInterface();
		ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();

		ShowErrorMessageEOS.Broadcast("Subsystem is valid; fetching successful");
	}
	else
		ShowErrorMessageEOS.Broadcast("Subsystem is invalid!");
}

void UEOSGameInstance::Init()
{
	Super::Init();

	USaveManager::Init(this);
	
	FetchOnlinePointers();

	if (Identity.IsValid() && SessionPtr.IsValid())
	{
		Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
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
	
	Identity->Login(0, Credentials);
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	Identity->ClearOnLoginCompleteDelegates(0, this);
	
	UE_LOG(LogTemp, Warning, TEXT("Login Delegate fired -> Success: %d"), bWasSuccessful)
	bIsLoggedIn = bWasSuccessful;
	if (!bWasSuccessful)
		ShowErrorMessageEOS.Broadcast(Error);

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

void UEOSGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	const FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	SessionPtr->ClearOnSessionUserInviteAcceptedDelegates(this);
	
	UE_LOG(LogOnlineIdentity, Warning, TEXT("Session User Invite Accepted -> Success: %d"), bWasSuccessful)
	if (!bWasSuccessful)
		return;

	ensureMsgf(InviteResult.IsValid(), TEXT("Invite Result is Invalid!!"));
	if (!InviteResult.IsValid())
		return;
	
	ShowErrorMessageEOS.Broadcast("Client has joined game");
	JoinGame(UserId, InviteResult);
}

void UEOSGameInstance::DestroySession()
{
	SessionPtr->DestroySession(DefaultSessionName);
}

void UEOSGameInstance::JoinGame(const FUniqueNetIdPtr UserID, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (!SessionToJoin.IsValid())
		return;

	SessionPtr->JoinSession(UserID->AsShared().Get(), DefaultSessionName, SessionToJoin);
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Delegate -> Success: %d"), (Result == EOnJoinSessionCompleteResult::Success) ? true : false);
	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);
	if (Result != EOnJoinSessionCompleteResult::Success)
		return;

	FString ConnectionInfo = FString();
	SessionPtr->GetResolvedConnectString(DefaultSessionName, ConnectionInfo);

	ensureMsgf(ConnectionInfo.IsEmpty(), TEXT("ConnectionInfo is Empty!!"));
	if (ConnectionInfo.IsEmpty())
		return;
	auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientTravel(ConnectionInfo, TRAVEL_Absolute);
}

void UEOSGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
	USaveManager::ClearSaveInterfaceArray();
	SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
}

void UEOSGameInstance::ShowInviteFriendsUI()
{
	if (!bIsLoggedIn)
		return;

	ExternalUIPtr->ShowInviteUI(0, DefaultSessionName);
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