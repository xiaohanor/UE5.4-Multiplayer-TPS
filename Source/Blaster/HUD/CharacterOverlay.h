// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ShieldBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreAmount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DefeatsAmount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedTeamScore;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BlueTeamScore;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreSpacerText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeaponAmmoAmount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CarriedAmmoAmount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MatchCountdownText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> GrenadesText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HighPingAnim;
};
