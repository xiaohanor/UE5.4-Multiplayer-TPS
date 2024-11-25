// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState: uint8
{
	EWS_Initial UMETA(DisplayName="Initial"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"),

	EWS_MAX UMETA(DisplayName="DefaultMAX")
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

	void ShowPickUpWidget(bool bShow);
	virtual void Fire(const FVector& HitTarget);

	/*准星纹理*/
	UPROPERTY(EditAnywhere,Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairCenter;

	UPROPERTY(EditAnywhere,Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairLeft;

	UPROPERTY(EditAnywhere,Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairRight;

	UPROPERTY(EditAnywhere,Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairTop;

	UPROPERTY(EditAnywhere,Category="Crosshair")
	TObjectPtr<UTexture2D> CrossHairBottom;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	
private:
	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState,VisibleAnywhere,Category="Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	TObjectPtr<UWidgetComponent> PickUpWidget;

	UPROPERTY(EditAnywhere,Category="Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;


public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE TObjectPtr<USphereComponent> AreaSphereGetter() const { return AreaSphere; }
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> WeaponMeshGetter() const { return WeaponMesh; }
};
