﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"

ATeamGameMode::ATeamGameMode()
{
	bTeamsMatch = true;
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState)
	{
		ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(NewPlayer->PlayerState);
		if (BlasterPlayerState && BlasterPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BlasterGameState->BlueTeamPlayers.Num() <= BlasterGameState->RedTeamPlayers.Num())
			{
				BlasterPlayerState->SetTeam(ETeam::ET_TeamBlue);
				BlasterGameState->BlueTeamPlayers.AddUnique(BlasterPlayerState);
			}
			else
			{
				BlasterPlayerState->SetTeam(ETeam::ET_TeamRed);
				BlasterGameState->RedTeamPlayers.AddUnique(BlasterPlayerState);
			}
		}
	}
}

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(Exiting->PlayerState);
	if (BlasterGameState && BlasterPlayerState)
	{
		if (BlasterGameState->BlueTeamPlayers.Contains(BlasterPlayerState))
		{
			BlasterGameState->BlueTeamPlayers.Remove(BlasterPlayerState);
		}
		if (BlasterGameState->RedTeamPlayers.Contains(BlasterPlayerState))
		{
			BlasterGameState->RedTeamPlayers.Remove(BlasterPlayerState);
		}
	}
}

float ATeamGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ABlasterPlayerState* AttackerPState = Cast<ABlasterPlayerState>(Attacker->PlayerState);
	ABlasterPlayerState* VictimPState = Cast<ABlasterPlayerState>(Victim->PlayerState);
	if (AttackerPState == nullptr || VictimPState == nullptr)
	{
		return BaseDamage;
	}
	if (AttackerPState == VictimPState)
	{
		return BaseDamage;
	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}

void ATeamGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
	ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
	
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	if (AttackerController == VictimController) return;
	if (BGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_TeamBlue)
		{
			BGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_TeamRed)
		{
			BGameState->RedTeamScores();
		}
	}
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState)
	{
		for (auto PState : BlasterGameState->PlayerArray)
		{
			ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(PState.Get());
			if (BlasterPlayerState && BlasterPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BlasterGameState->BlueTeamPlayers.Num() <= BlasterGameState->RedTeamPlayers.Num())
				{
					BlasterPlayerState->SetTeam(ETeam::ET_TeamBlue);
					BlasterGameState->BlueTeamPlayers.AddUnique(BlasterPlayerState);
				}
				else
				{
					BlasterPlayerState->SetTeam(ETeam::ET_TeamRed);
					BlasterGameState->RedTeamPlayers.AddUnique(BlasterPlayerState);
				}

			}
		}
	}
}

