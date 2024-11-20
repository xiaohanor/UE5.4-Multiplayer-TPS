// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

class AWeapon;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	TObjectPtr<ABlasterCharacter> BlasterCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	bool bWeaponEquipped;

	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess="true"))
	FTransform LeftHandTransform;


	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;
	
};
