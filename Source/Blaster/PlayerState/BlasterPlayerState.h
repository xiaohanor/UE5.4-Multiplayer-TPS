// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/BlasterTypes/Team.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	 ** 复制属性通知 
	 */
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();


	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(Replicated)
	ETeam Team = ETeam::ET_NoTeam;

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE void SetTeam(ETeam NewTeam) { Team = NewTeam; }
};
