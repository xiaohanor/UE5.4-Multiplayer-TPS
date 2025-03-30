// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Portal/Dashboard/CareerPage.h"
#include "ShooterCareerPage.generated.h"

class UCareerAchievement;
/**
 * 
 */
UCLASS()
class BLASTER_API UShooterCareerPage : public UCareerPage
{
	GENERATED_BODY()

public:
	virtual void OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse) override;
};
