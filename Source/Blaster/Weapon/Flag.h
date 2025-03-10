﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

UCLASS()
class BLASTER_API AFlag : public AWeapon
{
	GENERATED_BODY()

public:
	AFlag();
	virtual void Dropped() override;
	void ResetFlag();

protected:
	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> FlagMesh;

	FTransform InitializeTransform;

public:
	FORCEINLINE UStaticMeshComponent* GetFlagMesh() const { return FlagMesh; }
	FORCEINLINE FTransform GetInitializeTransform() const { return InitializeTransform; }
};
