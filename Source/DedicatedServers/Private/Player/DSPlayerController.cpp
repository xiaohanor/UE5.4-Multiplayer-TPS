// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSPlayerController.h"

ADSPlayerController::ADSPlayerController()
{
	SingleTripTime = 0.f;
	Username = "";
	PlayerSessionId = "";
}

void ADSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	// 仅在本地且联机模式上运行
	if (GetNetMode() == NM_Standalone) return;
	if (IsLocalController())
	{
		ServerPing(GetWorld()->GetTimeSeconds());
	}
}

void ADSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
 
	if (IsLocalController())
	{
		DisableInput(this);
	}
}
 
void ADSPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
 
	if (IsLocalController())
	{
		ServerPing(GetWorld()->GetTimeSeconds());
		DisableInput(this);
	}
}
 
void ADSPlayerController::BeginPlay()
{
	Super::BeginPlay();
 
	if (GetNetMode() == NM_Standalone)
	{
		DisableInput(this);
	}
}
 
void ADSPlayerController::Client_SetInputEnabled_Implementation(bool bEnabled)
{
	if (bEnabled)
	{
		EnableInput(this);
	}
	else
	{
		DisableInput(this);
	}
}

void ADSPlayerController::Client_TimerUpdated_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	OnTimerUpdated.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
}

void ADSPlayerController::Client_TimerStopped_Implementation(float CountdownTimeLeft, ECountdownTimerType Type) const
{
	OnTimerStopped.Broadcast(CountdownTimeLeft - SingleTripTime, Type);
}

void ADSPlayerController::ServerPing_Implementation(float TimeOfRequest)
{
	ClientPong(TimeOfRequest);
}

void ADSPlayerController::ClientPong_Implementation(float TimeOfRequest)
{
	// 计算单程时间
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfRequest;
	SingleTripTime = RoundTripTime * 0.5;
}

