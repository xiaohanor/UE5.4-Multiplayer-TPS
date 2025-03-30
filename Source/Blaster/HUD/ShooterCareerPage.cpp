// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCareerPage.h"

#include "Components/ScrollBox.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/Portal/Dashboard/CareerAchievement.h"

void UShooterCareerPage::OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse)
{
	Super::OnRetrieveMatchStats(RetrieveMatchStatsResponse);

	ScrollBox_Achievements->ClearChildren();
 
	TMap<FString, int32> AchievementData;

	if (RetrieveMatchStatsResponse.defeats > 0) AchievementData.Emplace(TEXT("战败："), RetrieveMatchStatsResponse.defeats);
	if (RetrieveMatchStatsResponse.scoredElims > 0) AchievementData.Emplace(TEXT("淘汰："), RetrieveMatchStatsResponse.scoredElims);

	for (const TPair<FString, int32>& Pair : AchievementData)
	{
		UCareerAchievement* CareerAchievement = CreateWidget<UCareerAchievement>(this, CareerAchievementClass);
		if (IsValid(CareerAchievement))
		{
			CareerAchievement->SetAchievementText(Pair.Key, Pair.Value);
			ScrollBox_Achievements->AddChild(CareerAchievement);
		}
	}
}
