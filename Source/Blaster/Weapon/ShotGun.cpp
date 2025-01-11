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
		// 对每个击中目标执行射线检测
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			// 执行武器的射线检测
			WeaponTraceHit(Start, HitTarget, FireHit);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				// 记录击中的角色及其被击中的次数
				// 如果已经击中过该角色，则增加其被击中的次数
				if (HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else
				{
					// 如果没有击中过该角色，则添加到击中列表中
					HitMap.Emplace(BlasterCharacter, 1);
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

		// 对每个被击中的角色应用伤害
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				if (HasAuthority())
				{
					UGameplayStatics::ApplyDamage(
					HitPair.Key,
					Damage * HitPair.Value,	// 伤害值乘以击中次数
					InstigatorController,
					this,
					UDamageType::StaticClass()
					);
				}
				HitCharacters.Add(HitPair.Key);
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? TObjectPtr<ABlasterCharacter>(Cast<ABlasterCharacter>(OwnerPawn)) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(InstigatorController)) : BlasterOwnerController;
			if (BlasterOwnerCharacter && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation())
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
