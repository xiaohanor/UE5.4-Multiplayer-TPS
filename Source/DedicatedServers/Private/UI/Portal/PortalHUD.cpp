// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Portal/Dashboard/DashboardOverlay.h"
#include "UI/Portal/SignIn/SignInOverlay.h"

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (IsValid(OwningPlayerController) && SignInOverlayClass)
	{
		SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
		SignInOverlay->AddToViewport();
	}

	FInputModeUIOnly InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);
	OwningPlayerController->SetShowMouseCursor(true);
}

void APortalHUD::SignIn()
{
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->RemoveFromParent();
	}
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (IsValid(OwningPlayerController) && DashboardOverlayClass)
	{
		DashboardOverlay = CreateWidget<UDashboardOverlay>(OwningPlayerController, DashboardOverlayClass);
		DashboardOverlay->AddToViewport();
	}
}

void APortalHUD::SignOut()
{
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->RemoveFromParent();
	}
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (IsValid(OwningPlayerController) && SignInOverlayClass)
	{
		SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass);
		SignInOverlay->AddToViewport();
	}
}

