// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

#include "UI/Portal/Interfaces/PortalManagement.h"

void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& AuthResult,
                                               TScriptInterface<IPortalManagement> PortalManagement)
{
	AuthenticationResult = AuthResult;
	PortalManagementInterface = PortalManagement;
	RefreshTokenTimer();
}

void UDSLocalPlayerSubsystem::RefreshTokenTimer()
{
	UWorld* World = GetWorld();
	if (IsValid(World) && PortalManagementInterface.GetObject())
	{
		FTimerDelegate RefreshDelegate;
		RefreshDelegate.BindLambda([this]{PortalManagementInterface->RefreshTokens(AuthenticationResult.RefreshToken);});
		World->GetTimerManager().SetTimer(TokenRefreshTimer, RefreshDelegate, TokenRefreshInterval, false);
	}
}

void UDSLocalPlayerSubsystem::UpdatedTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	RefreshTokenTimer();
}

FDSAuthenticationResult UDSLocalPlayerSubsystem::GetAuthResult() const
{
	return AuthenticationResult;
}
