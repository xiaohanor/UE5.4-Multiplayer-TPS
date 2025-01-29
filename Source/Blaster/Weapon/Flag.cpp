

#include "Flag.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


AFlag::AFlag()
{
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	SetRootComponent(FlagMesh);
	AreaSphereGetter()->SetupAttachment(FlagMesh);
	GetPickUpWidget()->SetupAttachment(FlagMesh);
	FlagMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


