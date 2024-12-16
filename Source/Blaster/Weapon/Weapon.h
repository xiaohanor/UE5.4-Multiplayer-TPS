// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState: uint8
{
	EWS_Initial UMETA(DisplayName="Initial"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName="Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName="Dropped"),

	EWS_MAX UMETA(DisplayName="DefaultMAX")
};

UENUM(BlueprintType)
enum class EFireType: uint8
{
	EFT_Projectile UMETA(DisplayName="Projectile Weapon"),
	EFT_HitScan UMETA(DisplayName="Hit Scan Weapon"),
	EFT_Shotgun UMETA(DisplayName="Shotgun Weapon"),

	EFT_MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner() override;
	void SetHUDAmmo();

	void ShowPickUpWidget(bool bShow);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

	/**
	 * 准星纹理
	 */
	UPROPERTY(EditAnywhere, Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairCenter;

	UPROPERTY(EditAnywhere, Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairLeft;

	UPROPERTY(EditAnywhere, Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairRight;

	UPROPERTY(EditAnywhere, Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairTop;

	UPROPERTY(EditAnywhere, Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairBottom;

	/**
	 * 瞄准时缩放FOV
	 */
	UPROPERTY(EditAnywhere, Category="FOV")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category="FOV")
	float ZoomInterpSpeed = 20.f;

	/**
	 * 自动开火
	 */
	UPROPERTY(EditAnywhere, Category="武器属性")
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category="武器属性")
	bool bAutomatic;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<USoundCue> EquippedSound;

	/**
	 * 启用或禁用自定义深度
	 */
	void EnableCustomDepth(bool bEnable);

	bool bDestroyWeapon = false;
	
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	bool bUseScatter = false;

	virtual FVector TraceEndWithScatter(const FVector& HitTarget);

protected:
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		
	/**
	 * 弹道散射
	 */
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float SphereRadius = 100.f;

private:
	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	TObjectPtr<UWidgetComponent> PickUpWidget;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo, Category="武器属性")
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category="武器属性")
	int32 MagCapacity;

	UPROPERTY(EditAnywhere, Category="武器属性")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category="武器属性")
	EFireType FireType;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterOwnerCharacter;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterOwnerController;

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE TObjectPtr<USphereComponent> AreaSphereGetter() const { return AreaSphere; }
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> WeaponMeshGetter() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty() const { return Ammo <= 0; }
	bool IsFull() const { return Ammo == MagCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EFireType GetFireType() const { return FireType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
};
