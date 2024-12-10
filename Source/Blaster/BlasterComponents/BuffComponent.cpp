// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"

#include "Blaster/Character/BlasterCharacter.h"


UBuffComponent::UBuffComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;	//每秒回血量
	AmountToHeal += HealAmount;	//总共回血量
}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UBuffComponent::HealRamUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed())
	{
		return;
	}

	const float HealThisFrame = HealingRate * DeltaTime;	//这一帧回血量
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();	//更新HUD
	AmountToHeal -= HealThisFrame;	//减去已经回血的量

	//回血结束
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}

}


void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRamUp(DeltaTime);
}

