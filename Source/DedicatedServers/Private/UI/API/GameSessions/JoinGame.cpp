// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/API/GameSessions/JoinGame.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJoinGame::SetStatusMessage(const FString& StatusMessage, bool bShouldResetJoinGameButton)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
	if (bShouldResetJoinGameButton)
	{
		Button_JoinGame->SetIsEnabled(true);
	}
}
