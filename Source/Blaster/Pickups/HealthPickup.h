﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class BLASTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	AHealthPickup();
	virtual void Destroyed() override;
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
	float HealthAmount = 50.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> PickupEffectComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickupEffect;
	

public:
};
