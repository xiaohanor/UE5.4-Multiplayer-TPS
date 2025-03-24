// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "DSLocalPlayerSubsystem.generated.h"

class IPortalManagement;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UDSLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	void InitializeTokens(const FDSAuthenticationResult& AuthResult, TScriptInterface<IPortalManagement> PortalManagement);
	void RefreshTokenTimer();
	void UpdatedTokens(const FString& AccessToken, const FString& IdToken);
	FDSAuthenticationResult GetAuthResult() const;

	FString Username{};
	FString Email{};
	FString Password{};
	
private:
	UPROPERTY()
	FDSAuthenticationResult AuthenticationResult;

	UPROPERTY()
	TScriptInterface<IPortalManagement> PortalManagementInterface;
	
	float TokenRefreshInterval = 2700.0f; // Token刷新间隔 45分钟
	FTimerHandle TokenRefreshTimer;
};
