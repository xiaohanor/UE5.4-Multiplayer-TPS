// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "GameFramework/Character.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include  "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrossHairInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

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

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();

	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION()
	void ReciveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                  AController* InstigatorController, AActor* DamageCauser);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	bool bDisableGameplay = false;

	//鼠标灵敏度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivity = 1.f;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScope(bool bIsAiming);

	void SpawnDefaultWeapon();

protected:
	virtual void BeginPlay() override;

	void AimOffset(float DeltaTime);
	void TurnInPlcae(float DeltaTime);
	void SimProxiesTurn();

	//轮询任何相关类并初始化我们的 HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBuffComponent> Buff;

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

	/**
	 * 溶解效果
	 */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//修改运行时的动态实例
	UPROPERTY(VisibleAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstanceDynamic> DynamicDissolveMaterialInstance;

	//在蓝图中设置的材质实例，用于动态材质实例
	UPROPERTY(EditAnywhere, Category="Elim")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	/**
	 * Eilm Bot
	 */

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> ElimBotComponent;

	/**
	 * 手榴弹
	 */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerState> BlasterPlayerState;

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
};
