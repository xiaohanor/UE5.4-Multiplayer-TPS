﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DS_MatchPlayerState.h"

#include "GameStats/GameStatsManager.h"

void ADS_MatchPlayerState::OnMatchEnded(const FString& Username)
{
	
}

void ADS_MatchPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
}

void ADS_MatchPlayerState::RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput) const
{
	check(IsValid(GameStatsManager));
	GameStatsManager->RecordMatchStats(RecordMatchStatsInput);
}
