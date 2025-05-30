// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}


ABlasterGameMode::ABlasterGameMode()
{
	
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	
	//如果玩家离开游戏，从最高得分玩家列表中删除
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}

void ABlasterGameMode::OnMatchEnded()
{
	Super::OnMatchEnded();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(Iterator->Get()); IsValid(PlayerController))
		{
			PlayerController->ClientHandleCooldown();
		}
	}

	TArray<FString> LeaderIds;
	if (ABlasterGameState* MatchGameState = GetGameState<ABlasterGameState>(); IsValid(MatchGameState))
	{
		TArray<ABlasterPlayerState*> Leaders = MatchGameState->TopScoringPlayers;
		for (ABlasterPlayerState* Leader : Leaders)
		{
			if (ABlasterPlayerController* LeaderPC = Cast<ABlasterPlayerController>(Leader->GetPlayerController()); IsValid(LeaderPC))
			{
				LeaderIds.Add(LeaderPC->Username);
			}
		}
	}
	UpdateLeaderboard(LeaderIds);
}

void ABlasterGameMode::OnMatchStarted()
{
	Super::OnMatchStarted();

	//通知所有玩家控制器比赛状态
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayerController)
		{
			BlasterPlayerController->ClientHandleMatchHasStarted();
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr)
	{
		return;
	}
	if (VictimController == nullptr || VictimController->PlayerState == nullptr)
	{
		return;
	}
	ABlasterPlayerState* AttackerPlayerState = AttackerController
		                                           ? Cast<ABlasterPlayerState>(AttackerController->PlayerState)
		                                           : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController
		                                         ? Cast<ABlasterPlayerState>(VictimController->PlayerState)
		                                         : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		TArray<ABlasterPlayerState*> PlayersCurrentLeading;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayersCurrentLeading.Add(LeadPlayer);
		}
		
		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		
		//如果玩家在最高得分玩家列表中，获得王冠
		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedCrown();
			}
		}
		//如果玩家不在最高得分玩家列表中，丢失王冠
		for (int32 i = 0; i < PlayersCurrentLeading.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentLeading[i]))
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentLeading[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostCrown();
				}
			}
		}
	}
		
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	//通知所有玩家控制器淘汰信息
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}

}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
