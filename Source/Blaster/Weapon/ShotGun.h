// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotGun.generated.h"

UCLASS()
class BLASTER_API AShotGun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	void ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);
	virtual void ShotgunFire(const TArray<FVector_NetQuantize>& HitTargets);
private:
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	uint32 NumOfPellets = 10; // 子弹数量
};
