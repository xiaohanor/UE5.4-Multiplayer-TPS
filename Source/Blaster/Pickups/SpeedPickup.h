﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

UCLASS()
class BLASTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:
	ASpeedPickup();
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 1300.f;

	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 800.f;	

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 20.f;


public:

};
