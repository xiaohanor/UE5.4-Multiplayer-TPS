// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/DSTypes.h"
#include "DSPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimerStateChangedDelegate, float, Time, ECountdownTimerType, Type);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADSPlayerController();
	virtual void ReceivedPlayer() override;	//当接收玩家时更快与服务器世界时钟同步
	virtual void OnRep_PlayerState() override;
	virtual void PostSeamlessTravel() override;
	virtual void BeginPlay() override;

	float SingleTripTime; // 单程时间

	UFUNCTION(Client, Reliable)
	void Client_TimerUpdated(float CountdownTimeLeft, ECountdownTimerType Type) const;
 
	UFUNCTION(Client, Reliable)
	void Client_TimerStopped(float CountdownTimeLeft, ECountdownTimerType Type) const;

	// 用于设置输入是否启用
	UFUNCTION(Client, Reliable)
	void Client_SetInputEnabled(bool bEnabled);
 
	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerUpdated;
 
	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerStopped;

	UPROPERTY(BlueprintReadOnly)
	FString Username;
 
	UPROPERTY(BlueprintReadOnly)
	FString PlayerSessionId;
	
protected:
	UFUNCTION(Server,Reliable)
	void ServerPing(float TimeOfRequest);

	UFUNCTION(Client,Reliable)
	void ClientPong(float TimeOfRequest);
};
