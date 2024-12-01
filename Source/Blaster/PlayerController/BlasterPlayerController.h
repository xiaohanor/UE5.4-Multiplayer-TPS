// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class UCharacterOverlay;
class ABlasterHUD;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 WeaponAmmo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float GetServerTime();	//与服务器世界时钟同步
	virtual void ReceivedPlayer() override;	//当接收玩家时更快与服务器世界时钟同步
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();
	
	/**
	 * 同步客户端与服务器时间
	 */

	//请求当前服务器时间，传入发送请求时客户端的时间
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClientRequest,float TimeOfServerReceivedClientRequset);

	float ClientServerDelta = 0.f;	//客户端与服务器时间差

	UPROPERTY(EditAnywhere,Category="Time")
	float TimeSyncFrequency = 5.f;	//时间同步频率

	float TimeSyncRunningTime = 0.f;	//时间同步运行时间
	void CheckTimeSync(float DeltaSeconds);

	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client,Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime);
private:
	UPROPERTY()
	TObjectPtr<ABlasterHUD> BlasterHUD;

	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	uint32 CountdownInt = 0;
	float LevelStartingTime;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;
	
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	
};
