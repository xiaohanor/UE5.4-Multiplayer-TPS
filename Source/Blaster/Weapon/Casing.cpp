// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	ShellEjectImpulse = 10.f;
}

void ACasing::SetInUse(bool InUse)
{
	Super::SetInUse(InUse);

	if (bInUse)
	{
		CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
		CasingMesh->SetSimulatePhysics(true);
		CasingMesh->SetEnableGravity(true);
		CasingMesh->SetNotifyRigidBodyCollision(true);
		bIsSoundPlaying = false;
		CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectImpulse);
	}
	else
	{
		CasingMesh->SetSimulatePhysics(false);
		CasingMesh->SetEnableGravity(false);
		CasingMesh->SetNotifyRigidBodyCollision(false);
		CasingMesh->OnComponentHit.RemoveDynamic(this, &ACasing::OnHit);
	}
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
                    FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound && !bIsSoundPlaying)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		bIsSoundPlaying = true;
	}
}
