// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotGun::ShotgunFire(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	// 获取武器的拥有者
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMeshGetter()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMeshGetter());
		FVector Start = SocketTransform.GetLocation();

		// 用于记录击中的角色及其被击中的次数
		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadShotHitMap;
		// 对每个击中目标执行射线检测
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			// 执行武器的射线检测
			WeaponTraceHit(Start, HitTarget, FireHit);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				// 如果击中的是头部，则造成头部伤害，否则造成普通伤害
				const bool bIsHeadShot = FireHit.BoneName == FName("head");
				if (bIsHeadShot)
				{
					if (HeadShotHitMap.Contains(BlasterCharacter)) HeadShotHitMap[BlasterCharacter]++;
					else HeadShotHitMap.Emplace(BlasterCharacter, 1);	// 如果没有击中过该角色，则添加到击中列表中
				}
				else
				{
					// 记录击中的角色及其被击中的次数
					// 如果已经击中过该角色，则增加其被击中的次数
					if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
					else HitMap.Emplace(BlasterCharacter, 1);	// 如果没有击中过该角色，则添加到击中列表中
				}
			}
			
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticle,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}

		TArray<ABlasterCharacter*> HitCharacters;
		// 用于记录每个被击中的角色及其受到的伤害
		TMap<ABlasterCharacter*, float> DamageMap;

		// 遍历击中列表，对每个被击中的角色造成身体伤害
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				
				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// 遍历击中列表，对每个被击中的角色造成头部伤害
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key && InstigatorController)
			{
				if (HitMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else HitMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);
				
				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		// 遍历伤害列表，对每个被击中的角色造成伤害
		for (auto DamagePair : DamageMap)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				UGameplayStatics::ApplyDamage(
				DamagePair.Key,
				DamagePair.Value,	// 造成的伤害
				InstigatorController,
				this,
				UDamageType::StaticClass()
				);
			}
		}
		
		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(OwnerPawn)) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(InstigatorController)) : BlasterOwnerController;
			if (BlasterOwnerCharacter && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensation()->ServerShotgunScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime);
			}
		}
	}
}


void AShotGun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMeshGetter()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMeshGetter());
	const FVector TraceStart = SocketTransform.GetLocation();
	
	// 归一化从起始位置到目标位置的方向向量
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	// 计算用于随机散射的球体中心
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i=0; i<NumOfPellets;++i)
	{
		// 在球体半径内生成一个随机向量
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		// 通过将随机向量添加到球体中心来计算终点位置
		const FVector EndLoc = SphereCenter + RandVec;
		// 计算从起始位置到终点位置的方向向量
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * HitScanTraceLength / ToEndLoc.Size();
			
		HitTargets.Add(ToEndLoc);
	}
}
