// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumOfPlayer = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem); // Make sure the subsystem is valid
		
		if (NumOfPlayer == Subsystem->DesiredConnections)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = false;
				FString MatchType = Subsystem->DesiredMatchType;
				if (MatchType == "FreeForAll")
				{
					GetWorld()->ServerTravel("/Game/Maps/BlasterMap?listen");
				}
				else if (MatchType == "Teams")
				{
					GetWorld()->ServerTravel("/Game/Maps/TeamsMap?listen");

				}
				else if (MatchType == "CTF")
				{
					GetWorld()->ServerTravel("/Game/Maps/CTFMap?listen");

				}
			}
		}
	}
	
	
}
