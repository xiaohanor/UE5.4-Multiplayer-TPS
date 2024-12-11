// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


class ABlasterCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();
	friend ABlasterCharacter;

	void Heal(float HealAmount, float HealingTime);
	void BuffSpeed(float BaseSpeedBuff, float CrouchSpeedBuff, float SpeedBuffTime);
	void InitializeSpeed(float BaseSpeed, float CrouchSpeed);

protected:
	virtual void BeginPlay() override;
	void HealRamUp(float DeltaTime);

private:
	TObjectPtr<ABlasterCharacter> Character;

	/*
	 * 生命值 Buff
	 */
	
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	/*
	 * 速度 Buff
	 */

	FTimerHandle SpeedBuffTimer;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	void ResetSpeed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
