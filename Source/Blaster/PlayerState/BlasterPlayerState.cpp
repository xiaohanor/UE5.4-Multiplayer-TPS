// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())): Character;
	if (Character)
	{
		Controller = Controller == nullptr ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Character->GetController())) : Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	int CurrentScore = GetScore();
	SetScore(CurrentScore + ScoreAmount); 
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetPawn())): Character;
	if (Character)
	{
		Controller = Controller == nullptr ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Character->GetController())) : Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}
