// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/Team.h"
#include "Player/DS_MatchPlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public ADS_MatchPlayerState
{
	GENERATED_BODY()

public:
	ABlasterPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnMatchEnded(const FString& Username) override;

	/*
	 ** 复制属性通知 
	 */
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

	
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	void SetTeam(ETeam TeamToSet);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	bool bWinner;

	UPROPERTY(ReplicatedUsing=OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
};
