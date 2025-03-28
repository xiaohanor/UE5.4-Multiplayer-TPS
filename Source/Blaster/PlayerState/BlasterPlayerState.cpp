// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/HTTP/HTTPRequestTypes.h"

ABlasterPlayerState::ABlasterPlayerState()
{
	Defeats = 0;
	SetScore(0);
	bWinner = false;
}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, Team);
}

void ABlasterPlayerState::OnMatchEnded(const FString& Username)
{
	Super::OnMatchEnded(Username);

	ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (IsValid(BlasterGameState))
	{
		bWinner = BlasterGameState->TopScoringPlayers.Contains(this);
	}
	
	FDSRecordMatchStatsInput RecordMatchStatsInput;
	RecordMatchStatsInput.username = Username;

	RecordMatchStatsInput.matchStats.defeats = Defeats;
	RecordMatchStatsInput.matchStats.scoredElims = GetScore();
	RecordMatchStatsInput.matchStats.matchWins = bWinner ? 1 : 0;
	RecordMatchStatsInput.matchStats.matchLosses = bWinner ? 0 : 1;
	RecordMatchStats(RecordMatchStatsInput);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Controller = Controller == nullptr
			             ? TObjectPtr<ABlasterPlayerController>(
				             Cast<ABlasterPlayerController>(Character->GetController()))
			             : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Controller = Controller == nullptr
			             ? TObjectPtr<ABlasterPlayerController>(
				             Cast<ABlasterPlayerController>(Character->GetController()))
			             : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Controller = Controller == nullptr
			             ? TObjectPtr<ABlasterPlayerController>(
				             Cast<ABlasterPlayerController>(Character->GetController()))
			             : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Character->SetTeamColor(Team);
	}
}

void ABlasterPlayerState::OnRep_Team()
{
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Character->SetTeamColor(Team);
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())) : Character;
	if (Character)
	{
		Controller = Controller == nullptr
			             ? TObjectPtr<ABlasterPlayerController>(
				             Cast<ABlasterPlayerController>(Character->GetController()))
			             : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
