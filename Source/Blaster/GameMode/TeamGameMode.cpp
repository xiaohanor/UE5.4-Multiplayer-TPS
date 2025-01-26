// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState)
	{
		ABlasterPlayerState* BlasterPlayerState = Cast<ABlasterPlayerState>(NewPlayer->PlayerState);
		if (BlasterPlayerState && BlasterPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BlasterGameState->BlueTeamScore <= BlasterGameState->RedTeamScore)
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
				if (BlasterGameState->BlueTeamScore <= BlasterGameState->RedTeamScore)
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
