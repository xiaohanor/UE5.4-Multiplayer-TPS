// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"

#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/Character/BlasterCharacter.h"


// Sets default values
AShieldPickup::AShieldPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuffComponent();
		if (Buff)
		{
			Buff->ShieldReplenish(ShieldReplenishAmount, ShieldReplenishTime);
		}
	}
	Destroy();
}



