// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/DSPlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterGameState;
class ABlasterPlayerState;
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
class BLASTER_API ABlasterPlayerController : public ADSPlayerController
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

	void HideTeamScores();
	void InitTeamScores();
	void SetHUDRedTeamScore(int32 Score);
	void SetHUDBlueTeamScore(int32 Score);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void SetupInputComponent() override;
	virtual void EnableInput(class APlayerController* PlayerController) override;
	virtual void DisableInput(class APlayerController* PlayerController) override;

	virtual float GetServerTime(); //与服务器世界时钟同步
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleMatchHasStarted(bool bTeamsMatch = false);
	UFUNCTION(Client, Reliable)
	void ClientHandleMatchHasStarted(bool bTeamsMatch = false);
	void HandleCooldown();
	UFUNCTION(Client, Reliable)
	void ClientHandleCooldown();

	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

protected:
	virtual void BeginPlay() override;
	void PollInit();
	
	UPROPERTY(EditAnywhere, Category="Time")
	float TimeSyncFrequency = 5.f; //时间同步频率

	float TimeSyncRunningTime = 0.f; //时间同步运行时间
	void CheckTimeSync(float DeltaSeconds);
	
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);

	void ShowReturnToMainMenu();

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();

	FString GetInfoText(const TArray<ABlasterPlayerState*>& Players);
	FString GetTeamsInfoText(const ABlasterGameState* BlasterGameState);
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
	int32 HUDGrenades;
	bool bInitializeGrenades = false;
	float HUDShield;
	bool bInitializeShield = false;
	float HUDMaxShield;
	bool bInitializeWeaponAmmo = false;
	int32 HUDWeaponAmmo;
	bool bInitializeCarriedAmmo = false;
	int32 HUDCarriedAmmo;
	bool bInitializeTeamScores = false;
	bool bInitializeMatchCountdown = false;
	float HUDMatchCountdown;

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
