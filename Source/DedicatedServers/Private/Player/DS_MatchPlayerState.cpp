// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DS_MatchPlayerState.h"

#include "GameStats/GameStatsManager.h"
#include "Player/DSLocalPlayerSubsystem.h"

void ADS_MatchPlayerState::OnMatchEnded(const FString& Username)
{
	// 由子类实现
}

void ADS_MatchPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);

	SetUserNameToPlayerName();
}

void ADS_MatchPlayerState::SetUserNameToPlayerName()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UDSLocalPlayerSubsystem>();
			SetPlayerName(DSLocalPlayerSubsystem->Username);
		}
	}
}

void ADS_MatchPlayerState::RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput) const
{
	check(IsValid(GameStatsManager));
	GameStatsManager->RecordMatchStats(RecordMatchStatsInput);
}
