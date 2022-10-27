// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
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
	ensureAlwaysMsgf(OnlineSubsystem != nullptr, TEXT("OnlineSubsytem is invalid!"));
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
		IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::OnLoginComplete);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		SessionPtr->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &ThisClass::OnSessionUserInviteAccepted);
		SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);
		SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete);
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

	SessionInfo.SessionName = FName(*UserId.ToString());
	CreateSession();
}

void UEOSGameInstance::CreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Create Session -> bIsLoggedIn: %d"), bIsLoggedIn)
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
	SessionSettings.Set(SEARCH_KEYWORDS, FString("GameLobby"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (EnsureOnlinePointersValidity())
		SessionPtr->CreateSession(0, SessionInfo.SessionName, SessionSettings);
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

	GetWorld()->ServerTravel("/Game/Levels/MainMenu?listen");
}

void UEOSGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum,
	const FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	SessionPtr->ClearOnSessionUserInviteAcceptedDelegates(this);
	DestroySession();
	
	UE_LOG(LogTemp, Warning, TEXT("Session User Invite Accepted -> Success: %d"), bWasSuccessful)
	if (!bWasSuccessful)
		return;

	ensureMsgf(InviteResult.IsValid(), TEXT("Invite Result is Invalid!!"));
	if (!InviteResult.IsValid())
		return;
	
	SessionInfo.SessionResult = InviteResult;
	SessionInfo.LocalUserNum = LocalUserNum;
	SessionInfo.UserId = UserId;

	// Wait for DestroySession and callback to finish so that JoinGame doesn't think user is still in session
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]{
		JoinGame(&SessionInfo);
	}, 2, false);
}

void UEOSGameInstance::JoinGame(const FSessionInfo* Session)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Game -> Session to join is Valid: %d"), Session->SessionResult.IsValid())
	if (!Session->SessionResult.IsValid())
		return;

	if (EnsureOnlinePointersValidity())
		SessionPtr->JoinSession(Session->LocalUserNum, Session->SessionName, Session->SessionResult);
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Delegate -> Success: %d"), (Result == EOnJoinSessionCompleteResult::Success) ? true : false);
	SessionPtr->ClearOnJoinSessionCompleteDelegates(this);
	if (Result != EOnJoinSessionCompleteResult::Success)
		return;

	FString ConnectString;
	if (!SessionPtr->GetResolvedConnectString(SessionName, ConnectString))
	{
		UE_LOG(LogTemp, Error, TEXT("ConnectionInfo is Empty!!"))
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Join session: traveling to %s"), *ConnectString);
	GetFirstLocalPlayerController()->ClientTravel(ConnectString, TRAVEL_Absolute, true);
}

void UEOSGameInstance::DestroySession()
{
	USaveManager::ClearSaveInterfaceArray();
	
	if (EnsureOnlinePointersValidity())
		SessionPtr->DestroySession(SessionInfo.SessionName);
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
		ExternalUIPtr->ShowInviteUI(0, SessionInfo.SessionName);
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
	USaveManager::ClearSaveInterfaceArray();
	
	if (GEngine != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GEngine is valid: %d"), GEngine != nullptr)
		GEngine->GameViewport->GetWorld()->ServerTravel("/Game/Levels/" + LevelAddress + "?listen", Absolute, ShouldSkipGameNotify);
	}
}

void UEOSGameInstance::ServerTravel(const FString& LevelAddress)
{
	USaveManager::ClearSaveInterfaceArray();
	const auto* const GameMode = UGameplayStatics::GetGameMode(GetWorld());

	GetWorld()->ServerTravel("/Game/Levels/" + LevelAddress + "?listen",true);
}
