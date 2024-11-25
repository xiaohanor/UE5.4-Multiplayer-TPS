// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include  "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

class UCombatComponent;
class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	void PlayerFireMontage(bool bAiming);

	
protected:
	virtual void BeginPlay() override;

	void AimOffset(float DeltaTime);
	void TurnInPlcae(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;

	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();
	
	//玩家输入映射
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputMappingContext> PlayerInputMapping;

	//玩家输入Action
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerMove;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerLook;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerJump;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> Equip;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerCrouch;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerSprint;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerAim;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerFire;

	//玩家输入函数
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	virtual void Jump() override;

	UPROPERTY(EditAnywhere)
	float SprintSpeed;
	UPROPERTY(EditAnywhere)
	float BaseSpeed;

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<UAnimMontage> FireWeaponMontage;

	
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


};
