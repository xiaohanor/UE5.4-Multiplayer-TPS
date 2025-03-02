// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPool.generated.h"


class APooledObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UObjectPool : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UObjectPool();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APooledObject> PooledObjectClass;

	UPROPERTY(EditDefaultsOnly)
	int PoolSize = 20;

	APooledObject* FindFirstAvailableActor();
	APooledObject* SpawnFromPool(const FTransform& Transform);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<APooledObject*> ObjectPool;

	void InitializePool();
};
