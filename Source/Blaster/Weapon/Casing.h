// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/ObjectPool/PooledObject.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class USoundCue;

UCLASS()
class BLASTER_API ACasing : public APooledObject
{
	GENERATED_BODY()

public:
	ACasing();

protected:
	virtual void SetInUse(bool InUse) override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	                   FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectImpulse;

	UPROPERTY(EditAnywhere)
	USoundCue* ShellSound;

	bool bIsSoundPlaying = false;
};
