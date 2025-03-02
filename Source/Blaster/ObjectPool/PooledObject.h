// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledObject.generated.h"

UCLASS()
class BLASTER_API APooledObject : public AActor
{
	GENERATED_BODY()

public:
	bool bInUse;

	UPROPERTY(EditDefaultsOnly)
	float TimeToLive = 3.f;

	virtual void SetInUse(bool InUse);
protected:
	virtual void BeginPlay() override;
	void ReturnToPool();

private:
	FTimerHandle TimeToLiveHandle;
};
