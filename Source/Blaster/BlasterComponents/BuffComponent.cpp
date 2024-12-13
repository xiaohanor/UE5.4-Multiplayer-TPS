// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRamUp(DeltaTime);
	ShieldRamUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime; //每秒回血量
	AmountToHeal += HealAmount; //总共回血量
}

void UBuffComponent::ShieldReplenish(float ShieldAmount, float ReplenishTime)
{
	bReplenishingShield = true;
	ShieldReplenishRate = ShieldAmount / ReplenishTime;
	ShieldReplenishAmount += ShieldAmount;
}

void UBuffComponent::HealRamUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed())
	{
		return;
	}

	const float HealThisFrame = HealingRate * DeltaTime; //这一帧回血量
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth(); //更新HUD
	AmountToHeal -= HealThisFrame; //减去已经回血的量

	//回血结束
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ShieldRamUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed())
	{
		return;
	}

	const float ShieldThisFrame = ShieldReplenishRate * DeltaTime; //这一帧回护盾量
	Character->SetShield(FMath::Clamp(Character->GetShield() + ShieldThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield(); //更新HUD
	ShieldReplenishAmount -= ShieldThisFrame; //减去已经回护盾的量

	//回护盾结束
	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
	}
}

void UBuffComponent::InitializeSpeed(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::BuffSpeed(float BaseSpeedBuff, float CrouchSpeedBuff, float SpeedBuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeed,
		SpeedBuffTime
	);
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeedBuff;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeedBuff;
	MulticastSpeedBuff(BaseSpeedBuff, CrouchSpeedBuff);
}

void UBuffComponent::ResetSpeed()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}
