// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class UReturnToMainMenu;
class UInputAction;
class ABlasterGameMode;
class UCharacterOverlay;
class ABlasterHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bHighPing);

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 WeaponAmmo);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;

	virtual float GetServerTime(); //与服务器世界时钟同步
	virtual void ReceivedPlayer() override; //当接收玩家时更快与服务器世界时钟同步
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	float SingleTripTime = 0.f; //单程时间

	FHighPingDelegate HighPingDelegate;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	/**
	 * 同步客户端与服务器时间
	 */

	//请求当前服务器时间，传入发送请求时客户端的时间
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeOfServerReceivedClientRequset);

	float ClientServerDelta = 0.f; //客户端与服务器时间差

	UPROPERTY(EditAnywhere, Category="Time")
	float TimeSyncFrequency = 5.f; //时间同步频率

	float TimeSyncRunningTime = 0.f; //时间同步运行时间
	void CheckTimeSync(float DeltaSeconds);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);
	
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);

	void ShowReturnToMainMenu();

private:
	UPROPERTY()
	TObjectPtr<ABlasterHUD> BlasterHUD;

	/**
	 * 返回主菜单
	 */

	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	TObjectPtr<UReturnToMainMenu> ReturnToMainMenu;

	bool bReturnToMainMenu = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> Quit;

	
	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;

	float HUDHealth;
	bool bInitializeHealth = false;
	float HUDMaxHealth;
	float HUDScore;
	bool bInitializeScore = false;
	int32 HUDDefeats;
	bool bInitializeDefeats = false;
	int32 HUDGreandes;
	bool bInitializeGrenades = false;
	float HUDShield;
	bool bInitializeShield = false;
	float HUDMaxShield;
	bool bInitializeWeaponAmmo = false;
	int32 HUDWeaponAmmo;
	bool bInitializeCarriedAmmo = false;
	int32 HUDCarriedAmmo;

	float HighPingRunningTime = 0.f;
	float HighPingAnimRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	UPROPERTY(EditAnywhere)
	float CheckHighPingFrequency = 20.f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 100.f;
};
