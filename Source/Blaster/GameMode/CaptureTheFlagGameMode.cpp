// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagGameMode.h"

#include "Blaster/BlasterTypes/Team.h"
#include "Blaster/CaptureTheFlag/FlagZone.h"
#include "Blaster/GameState/BlasterGameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter,
                                               ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* FlagZone)
{
	ABlasterGameState* BGameState = GetWorld()->GetGameState<ABlasterGameState>();
	if (BGameState)
	{
		if (FlagZone->Team == ETeam::ET_TeamBlue)
		{
			BGameState->BlueTeamScores();
		}
		if (FlagZone->Team == ETeam::ET_TeamRed)
		{
			BGameState->RedTeamScores();
		}
	}
}
