// Fill out your copyright notice in the Description page of Project Settings.


#include "NETSaveGame.h"

#include "GameFramework/Character.h"

UNETSaveGame::UNETSaveGame()
{
	ServerSlotName = TEXT("TestServerSlot");
	UserIndex = 0;
	PlayerLocation = FVector(0.0f, 0.0f, 0.0f);
}

void UNETSaveGame::SaveLocalPlayerData(const ACharacter* LocalCharacter)
{
	PlayerLocation = LocalCharacter->GetActorLocation();
}

void UNETSaveGame::LoadLocalPlayerData(const ACharacter* LocalCharacter)
{
	LocalCharacter->GetActorLocation() = PlayerLocation;
}
