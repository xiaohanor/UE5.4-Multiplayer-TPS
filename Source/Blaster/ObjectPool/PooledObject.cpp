// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledObject.h"

void APooledObject::BeginPlay()
{
	Super::BeginPlay();
	SetInUse(false);
}

void APooledObject::SetInUse(bool InUse)
{
	bInUse = InUse;
	SetActorEnableCollision(bInUse);
	SetActorHiddenInGame(!bInUse);
	SetActorTickEnabled(bInUse);
	GetWorldTimerManager().ClearTimer(TimeToLiveHandle);
	if (bInUse)
	{
		GetWorldTimerManager().SetTimer(TimeToLiveHandle, this, &APooledObject::ReturnToPool, TimeToLive, false);
	}
}

void APooledObject::ReturnToPool()
{
	SetInUse(false);
}
