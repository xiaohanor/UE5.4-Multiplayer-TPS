﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/SignIn/SignInOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/Portal/PortalManager.h"
#include "UI/Portal/SignIn/ConfirmSignUpPage.h"
#include "UI/Portal/SignIn/SignInPage.h"
#include "UI/Portal/SignIn/SignUpPage.h"
#include "UI/Portal/SignIn/SuccessConfirmPage.h"


void USignInOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(PortalManagerClass);
	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);

	SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &USignInOverlay::SignInButtonClicked);
	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	SignInPage->Button_Quit->OnClicked.AddDynamic(PortalManager, &UPortalManager::QuitGame);
	PortalManager->SignInStatusMessageDelegate.AddDynamic(SignInPage, &USignInPage::UpdateStatusMessage);
	
	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::USignInOverlay::ShowSignInPage);
	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &USignInOverlay::SignUpButtonClicked);
	PortalManager->SignUpStatusMessageDelegate.AddDynamic(SignUpPage, &USignUpPage::UpdateStatusMessage);
	PortalManager->SignUpSucceededDelegate.AddDynamic(this, &USignInOverlay::OnSignUpSucceeded);
	
	ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &USignInOverlay::ConfirmButtonClicked);
	ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignUpPage);
	PortalManager->ConfirmStatusMessageDelegate.AddDynamic(ConfirmSignUpPage, &UConfirmSignUpPage::UpdateStatusMessage);
	PortalManager->ConfirmSucceededDelegate.AddDynamic(this, &USignInOverlay::OnConfirmSucceeded);
	
	SuccessConfirmedPage->Button_Ok->OnClicked.AddDynamic(this, &USignInOverlay::ShowSignInPage);

	ShowSignInPage();
	AutoSignIn();
}

void USignInOverlay::AutoSignIn()
{
	if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = PortalManager->GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
	{
		const FString& Username = DSLocalPlayerSubsystem->Username;
		const FString& Password = DSLocalPlayerSubsystem->Password;
		if (Username.IsEmpty() || Password.IsEmpty()) return;
 
		SignInPage->Button_SignIn->SetIsEnabled(false);
		PortalManager->SignIn(Username, Password);
	}
}

void USignInOverlay::ShowSignInPage()
{
	SignInPage->Button_SignIn->SetIsEnabled(true);
	WidgetSwitcher->SetActiveWidget(SignInPage);
}

void USignInOverlay::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(SignUpPage);
}

void USignInOverlay::ShowConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(ConfirmSignUpPage);
}

void USignInOverlay::ShowSuccessConfirmedPage()
{
	WidgetSwitcher->SetActiveWidget(SuccessConfirmedPage);
}

void USignInOverlay::SignInButtonClicked()
{
	const FString Username = SignInPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();
	SignInPage->Button_SignIn->SetIsEnabled(false);
	if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = PortalManager->GetDSLocalPlayerSubsystem(); IsValid(DSLocalPlayerSubsystem))
	{
		DSLocalPlayerSubsystem->Password = Password;
	}
	PortalManager->SignIn(Username, Password);
}

void USignInOverlay::SignUpButtonClicked()
{
	const FString Username = SignUpPage->TextBox_UserName->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	SignUpPage->Button_SignUp->SetIsEnabled(false);
	PortalManager->SignUp(Username, Password, Email);
}

void USignInOverlay::ConfirmButtonClicked()
{
	const FString ConfirmationCode = ConfirmSignUpPage->TextBox_ConfirmationCode->GetText().ToString();
	ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	PortalManager->Confirm(ConfirmationCode);
}

void USignInOverlay::OnSignUpSucceeded()
{
	SignUpPage->ClearTextBoxes();
	ConfirmSignUpPage->TextBlock_Destination->SetText(FText::FromString(PortalManager->LastSignUpResponse.CodeDeliveryDetails.Destination));
	ShowConfirmSignUpPage();
}

void USignInOverlay::OnConfirmSucceeded()
{
	ConfirmSignUpPage->ClearTextBoxes();
	ShowSuccessConfirmedPage();
}
