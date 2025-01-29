// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"


class AFlag;
class AProjectile;
class ABlasterPlayerController;
class AWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();

	UFUNCTION(BlueprintCallable)
	void FinishReload();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	/*UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();*/

	void JumpToShotgunEnd();

	UFUNCTION(blueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(blueprintCallable)
	void GrenadeLaunched();

	void FireButtonPressed(bool bPressed);

	bool bLocallyReloading = false;

	void PickupAmmo(EWeaponType WeaponType, int32 Amount);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	
	void Reload();
	
	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();
	

	void Fire();
	void FireProjectileWeapon();
	void FireHitscanWeapon();
	void FireShotgunWeapon();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void ThrowGrenade();
	
	UFUNCTION(Server,Reliable)
	void ServerThrowGrenade();

	UFUNCTION(Server,Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> GrenadeClass;

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachFlagToLeftHand(AWeapon* FlagToAttach);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void ShowAttachedGrenade(bool bShow);
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	void AttachActorToBackpack(AActor* ActorToAttach);
	
private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;
	UPROPERTY()
	TObjectPtr<ABlasterHUD> HUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(ReplicatedUsing=OnRep_SecondaryWeapon)
	TObjectPtr<AWeapon> SecondaryWeapon;

	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool bAiming=false;

	bool bAimButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
	float AimSpeed;

	bool bFireButtonPressed;

	/*HUD和准星*/
	float CrossHairVelocityFactor;
	float CrossHairInAirFactor;
	float CrossHairAimFactor;
	float CrossHairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	/*瞄准和FOV*/
	float DefaultFOV;
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category="FOV")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category="FOV")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/*自动开火*/
	FTimerHandle FireTimer;

	void StartFireTimer();
	void FireTImerFinished();

	bool bCanFire = true;

	bool CanFire();

	/*当前携带武器的弹药*/
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 400;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingShotGunAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeAmmo = 0;

	TMap<EWeaponType, int32> CarriedAmmoMap;

	void initializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
	
	void UpdateAmmoValues();
	void UpdateShotGunAmmoValues();

	/*
	 * 手雷
	 */
	UPROPERTY(ReplicatedUsing=OnRep_Grenades)
	int32 Grenades = 4;

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;

	void UpdateGrenades();

	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
	bool bHoldingTheFlag = false;

	UFUNCTION()
	void OnRep_HoldingTheFlag();

public:
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	FORCEINLINE bool ShouldSwapWeapons() const {return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);};
};
