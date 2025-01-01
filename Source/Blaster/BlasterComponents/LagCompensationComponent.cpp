﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "Blaster/Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"


ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	FFramePackage Package;
	SaveFramePackage(Package);
	ShowFramePackage(Package, FColor::Orange);
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& FramePackage)
{
	Character = Character == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(GetOwner())) : Character;
	if (Character)
	{
		FramePackage.Time = Character->GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInfo;
			BoxInfo.Location = BoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			FramePackage.HitBoxInfo.Add(BoxPair.Value->GetFName(), BoxInfo);
		}
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& FramePackage, const FColor Color)
{
	for (auto& BoxInfo : FramePackage.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			true
		);
	}
}

