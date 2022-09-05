// Fill out your copyright notice in the Description page of Project Settings.


#include "NETSaveGame.h"

#include "Kismet/GameplayStatics.h"

UNETSaveGame::UNETSaveGame()
{
	ServerSlotName = TEXT("TestServerSlot");
	UserIndex = 0;
	PlayerLocation = FVector(0.0f, 0.0f, 0.0f);
	SavedLevelName = "";
}

void UNETSaveGame::SaveLocalPlayerData(APlayerCharacter* LocalCharacter)
{
	PlayerLocation = LocalCharacter->GetActorLocation();
	SavedLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
}

void UNETSaveGame::LoadLocalPlayerData(APlayerCharacter* LocalCharacter)
{
	LocalCharacter->SetActorLocation(PlayerLocation);
	GetWorld()->ServerTravel(SavedLevelName);
}
