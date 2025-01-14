// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = WeaponMeshGetter()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(WeaponMeshGetter());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile* SpawnedProjectile = nullptr;
		if (bUseServerSideRewind)
		{
			if (InstigatorPawn->HasAuthority())	// server
			{
				if (InstigatorPawn->IsLocallyControlled())	// server, host - 使用复制的子弹
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
				}
				else // server, 非本地控制 - 生成无复制的子弹，使用服务器倒带
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
				}
			}
			else // client, 使用服务器倒带
			{
				if (InstigatorPawn->IsLocallyControlled())	// client, 本地控制 - 生成无复制的子弹，使用服务器倒带
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = ToTarget.GetSafeNormal() * SpawnedProjectile->InitialSpeed;
					SpawnedProjectile->Damage = Damage;
				}
				else // client, 非本地控制 - 生成无复制的子弹，无服务器倒带
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),TargetRotation, SpawnParams);
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else // 武器不使用服务器倒带
		{
			if (HasAuthority())	// server
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),TargetRotation, SpawnParams);
				SpawnedProjectile->bUseServerSideRewind = false;
				SpawnedProjectile->Damage = Damage;
			}
		}
	}
}
