// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnToMainMenu.h"

#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/GameModeBase.h"

void UReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (IsValid(PlayerController))
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (IsValid(MultiplayerSessionsSubsystem) && !MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
		{
			UE_LOG(LogTemp, Warning, TEXT("Binding OnDestroySessionComplete"));
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnSessionDestroyed);
		}
	}
}

bool UReturnToMainMenu::Initialize()
{
	return Super::Initialize();
}

void UReturnToMainMenu::OnSessionDestroyed(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Destroyed"));
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		// If we are the host, we can return to the main menu directly
		if (IsValid(GameMode))
		{
			GameMode->ReturnToMainMenuHost();
		}
		else // If we are a client, we need to ask the host to return to the main menu
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (IsValid(PlayerController))
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (IsValid(PlayerController))
		{
			FInputModeGameAndUI InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}
	if (IsValid(MultiplayerSessionsSubsystem) && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnSessionDestroyed);
	}
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if (IsValid(FirstPlayerController))
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (IsValid(BlasterCharacter))
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerLeftGame()"))

	if (IsValid(MultiplayerSessionsSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessionsSubsystem valid"))
		MultiplayerSessionsSubsystem->DestroySession();
	}
}
