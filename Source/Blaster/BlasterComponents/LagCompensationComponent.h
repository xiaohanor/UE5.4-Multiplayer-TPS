﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	ABlasterCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ABlasterCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<ABlasterCharacter*, uint32> BodyShots;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABlasterCharacter;
	void ShowFramePackage(const FFramePackage& FramePackage, const FColor Color);

	/*
	 * Hitscan
	 */
	FServerSideRewindResult ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                         const FVector_NetQuantize& HitLocation, float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
							const FVector_NetQuantize& HitLocation, float HitTime);

	/*
	 * Projectile
	 */
	FServerSideRewindResult ProjectileServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                                   const FVector_NetQuantize100& InitialVelocity, float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerProjectileScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                  const FVector_NetQuantize100& InitialVelocity, float HitTime);
	/*
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<ABlasterCharacter*>& HitCharacters,
															 const FVector_NetQuantize& TraceStart,
															 const TArray<FVector_NetQuantize>& HitLocations,
															 float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerShotgunScoreRequest(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	                               const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& FramePackage);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame,
	                                  float HitTime);
	void CacheBoxPosition(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);
	void ResetBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterCollision(ABlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionType);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime);

	/*
	 * Hitscan
	 */
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter,
								   const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);

	/*
	 * Projectile
	 */
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                             const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/*
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages,
	                                                 const FVector_NetQuantize& TraceStart,
	                                                 const TArray<FVector_NetQuantize>& HitLocations);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;

	// 帧历史
	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
};
