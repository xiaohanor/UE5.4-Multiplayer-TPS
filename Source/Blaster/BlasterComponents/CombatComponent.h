// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"


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

	UFUNCTION(BlueprintCallable)
	void FinishReload();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(blueprintCallable)
	void ThrowGrenadeFinished();

	void FireButtonPressed(bool bPressed);

	bool bLocallyReloading = false;

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	
	void Reload();
	
	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void ThrowGrenade();
	
	UFUNCTION(Server,Reliable)
	void ServerThrowGrenade();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound();
	void ReloadEmptyWeapon();
	
private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;
	UPROPERTY()
	TObjectPtr<ABlasterHUD> HUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

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

	//当前携带武器的弹药
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

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

};
