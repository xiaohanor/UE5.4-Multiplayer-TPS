// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "GameFramework/Character.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include  "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrossHairInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

class ABlasterGameMode;
class UNiagaraComponent;
class UNiagaraSystem;
class ULagCompensationComponent;
class UBoxComponent;
class ABlasterPlayerState;
class FOnTimelineFloat;
class UTimelineComponent;
class ABlasterPlayerController;
class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrossHairInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	void CalculateAO_Pitch();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
/*
 * 播放动画蒙太奇
 */
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapWeaponMontage();

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                  AController* InstigatorController, AActor* DamageCauser);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void UpdateHUDGrenades();

	void Elim(bool bPlayerLeft);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeft);

	bool bDisableGameplay = false;

	//鼠标灵敏度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivity = 1.f;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScope(bool bIsAiming);

	void SpawnDefaultWeapon();

	bool bFinishedSwapping = false;

	UPROPERTY()
	TMap<FName,UBoxComponent*> HitCollisionBoxes;

	UFUNCTION(Server,Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedCrown();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostCrown();

	void SetTeamColor(ETeam Team);

protected:
	virtual void BeginPlay() override;
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	void AimOffset(float DeltaTime);
	void TurnInPlace(float DeltaTime);
	void SimProxiesTurn();

	//轮询任何相关类并初始化我们的 HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	/*
	 * 用于服务器端倒带的 HitBox
	 */

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> head;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> pelvis;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_02;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_03;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> upperarm_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> upperarm_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> hand_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> hand_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> backpack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> blanket;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> thigh_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> thigh_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> foot_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> foot_r;

private:
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/*
	 * 组件
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBuffComponent> Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ULagCompensationComponent> LagCompensation;
	

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterPlayerController;

	//玩家输入映射
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputMappingContext> PlayerInputMapping;

	/**
	 * 玩家输入Action
	 */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerMove;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerLook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerJump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> Equip;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerSprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerAim;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> PlayerFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> Reload;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerInput", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputAction> ThrowGrenade;

	/**
	 * 玩家输入函数
	 */

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	void ThrowGrenadeButtonPressed();

	UPROPERTY(EditAnywhere)
	float SprintSpeed;
	UPROPERTY(EditAnywhere)
	float BaseSpeed;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	ETurningInPlace TurningInPlace;

	/**
	 * 动画蒙太奇
	 */

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> ElimMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> SwapWeaponMontage;

	/**
	 * 玩家生命值
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/**
	 * 玩家护盾值
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_Shield, VisibleAnywhere, Category="Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;
	
	/**
	 * 溶解效果
	 */

	UPROPERTY(VisibleAnywhere, Category="Elim")
	TObjectPtr<UTimelineComponent> DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(VisibleAnywhere, Category="Elim")
	TObjectPtr<UCurveFloat> DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	
	void StartDissolve();

	/**
	 * 队伍颜色
	 */

	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> RedDissolveMatIns;

	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> RedMaterial;

	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> BlueDissolveMatIns;

	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> BlueMaterial;
	
	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> OriginalMaterial;


	//修改运行时的动态实例
	UPROPERTY(VisibleAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstanceDynamic> DynamicDissolveMaterialInstance;

	//在蓝图中设置的材质实例，用于动态材质实例
	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	/**
	 * Eilm Effect
	 */

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> ElimBotComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> CrownSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> CrownComponent;

	/**
	 * 手榴弹
	 */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerState> BlasterPlayerState;

	UPROPERTY()
	TObjectPtr<ABlasterGameMode> BlasterGameMode;

	/**
	* 初始武器
	*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	AWeapon* GetEquippedWeapon();
	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE float GetBaseSpeed() const { return BaseSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return Buff; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	bool IsLocallyReloading() const;
	bool IsHoldingTheFlag() const;
	ETeam GetTeam();
	void SetHoldingTheFlag(bool bHold);
	void SetTheFlag(bool bHold);
};
