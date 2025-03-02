// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool.h"

#include "PooledObject.h"


// Sets default values for this component's properties
UObjectPool::UObjectPool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


APooledObject* UObjectPool::FindFirstAvailableActor()
{
	if (ObjectPool.Num() == 0)
	{
		return nullptr;
	}
	
	for (APooledObject* Actor : ObjectPool)
	{
		if (Actor && !Actor->bInUse)
		{
			return Actor;
		}
	}
	return nullptr;
}

APooledObject* UObjectPool::SpawnFromPool(const FTransform& Transform)
{
	APooledObject* PooledObject = FindFirstAvailableActor();
	if (IsValid(PooledObject))
	{
		// 使用TeleportPhysics确保物理引擎更新位置
		PooledObject->SetActorTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
		PooledObject->SetInUse(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No available actors in the pool"));
	}
	return PooledObject;
}

// Called when the game starts
void UObjectPool::BeginPlay()
{
	Super::BeginPlay();

	InitializePool();
}

void UObjectPool::InitializePool()
{
	if (IsValid(PooledObjectClass))
	{
		for (int i = 0; i<PoolSize; i++)
		{
			APooledObject* PooledObject = GetWorld()->SpawnActor<APooledObject>(PooledObjectClass);
			ObjectPool.AddUnique(PooledObject);
			PooledObject->SetInUse(false);
		}
	}
}


