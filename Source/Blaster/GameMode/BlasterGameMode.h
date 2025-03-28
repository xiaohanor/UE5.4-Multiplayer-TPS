// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Game/DS_MatchGameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterPlayerState;

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; //比赛时间结束，显示获胜者并进入冷却时间
}


class ABlasterPlayerController;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public ADS_MatchGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();
	virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
	                              ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	virtual void OnMatchStateSet() override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	void PlayerLeftGame(ABlasterPlayerState* PlayerLeaving);

	bool bTeamsMatch = false;

protected:
	virtual void OnMatchEnded() override;
};
