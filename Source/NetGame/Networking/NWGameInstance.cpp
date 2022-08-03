// Fill out your copyright notice in the Description page of Project Settings.


#include "NWGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

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
	}
}

void UNWGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Delegate fired -> Success: %d"), Succeeded)
	if (Succeeded)
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
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
	
}
