// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumOfPublicConnections = 4, FString MatchOfType = FString(TEXT("XIAOHAN")),
	               FString PathToLobby = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

	UPROPERTY(meta=(BindWidget))
	class UButton* HostButton;

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//用于MultiplayerSessionsSubsystem类的委托绑定的回调函数
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTeardown();

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	int32 NumPublicConnections{4};
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FString MatchType{TEXT("XIAOHAN")};
	
	FString LobbyPath{TEXT("")};;
};
