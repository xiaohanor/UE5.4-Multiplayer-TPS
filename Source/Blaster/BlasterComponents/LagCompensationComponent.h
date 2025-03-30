// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;

/**
 * 碰撞盒信息结构体
 * 存储碰撞盒的位置、旋转和尺寸信息
 */
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

/**
 * 帧包结构体
 * 存储特定时间点的角色碰撞盒信息
 * 用于服务器端回溯功能
 */
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

/**
 * 服务器端回溯结果结构体
 * 存储命中确认和是否为爆头的信息
 */
USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

/**
 * 散弹枪服务器端回溯结果结构体
 * 存储每个被击中角色的爆头和身体命中次数
 */
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

	/**
	 * 显示帧包数据（调试用）
	 * @param FramePackage 要显示的帧包
	 * @param Color 显示颜色
	 */
	void ShowFramePackage(const FFramePackage& FramePackage, const FColor Color);

	/**
	 * 即时射击武器的服务器端回溯
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param HitLocation 击中位置
	 * @param HitTime 击中时间
	 * @return 回溯结果，包含命中确认和爆头信息
	 */
	FServerSideRewindResult ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                         const FVector_NetQuantize& HitLocation, float HitTime);

	/**
	 * 客户端请求服务器确认即时射击命中
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param HitLocation 击中位置
	 * @param HitTime 击中时间
	 */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
							const FVector_NetQuantize& HitLocation, float HitTime);

	/**
	 * 弹道武器的服务器端回溯
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param InitialVelocity 初始速度
	 * @param HitTime 击中时间
	 * @return 回溯结果，包含命中确认和爆头信息
	 */
	FServerSideRewindResult ProjectileServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                                   const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/**
	 * 客户端请求服务器确认弹道武器命中
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param InitialVelocity 初始速度
	 * @param HitTime 击中时间
	 */
	UFUNCTION(Server, Reliable)
	void ServerProjectileScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                  const FVector_NetQuantize100& InitialVelocity, float HitTime);
	
	/**
	 * 散弹枪的服务器端回溯
	 * @param HitCharacters 被击中的角色数组
	 * @param TraceStart 射线起始点
	 * @param HitLocations 击中位置数组
	 * @param HitTime 击中时间
	 * @return 散弹枪回溯结果，包含每个角色的爆头和身体命中次数
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<ABlasterCharacter*>& HitCharacters,
															 const FVector_NetQuantize& TraceStart,
															 const TArray<FVector_NetQuantize>& HitLocations,
															 float HitTime);
	/**
	 * 客户端请求服务器确认散弹枪命中
	 * @param HitCharacters 被击中的角色数组
	 * @param TraceStart 射线起始点
	 * @param HitLocations 击中位置数组
	 * @param HitTime 击中时间
	 */
	UFUNCTION(Server, Reliable)
	void ServerShotgunScoreRequest(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	                               const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

protected:
	virtual void BeginPlay() override;
	
	/**
	 * 保存角色当前状态到帧包中
	 * @param FramePackage 输出的帧包
	 */
	void SaveFramePackage(FFramePackage& FramePackage);

	/**
	 * 在两个帧之间进行插值，获取特定时间点的帧状态
	 * @param OlderFrame 较早的帧
	 * @param YoungerFrame 较新的帧
	 * @param HitTime 目标时间点
	 * @return 插值后的帧包
	 */	
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame,
	                                  float HitTime);

	/**
	 * 缓存角色碰撞盒位置信息
	 * @param HitCharacter 目标角色
	 * @param OutFramePackage 输出的帧包
	 */	
	void CacheBoxPosition(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage);

	/**
	 * 移动角色碰撞盒到帧包中记录的位置
	 * @param HitCharacter 目标角色
	 * @param Package 包含位置信息的帧包
	 */	
	void MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);

	/**
	 * 重置角色碰撞盒到原始位置
	 * @param HitCharacter 目标角色
	 * @param Package 包含原始位置的帧包
	 */	
	void ResetBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);

	/**
	 * 设置角色碰撞类型
	 * @param HitCharacter 目标角色
	 * @param CollisionType 碰撞类型
	 */	
	void EnableCharacterCollision(ABlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionType);

	/**
	 * 保存当前帧到帧历史
	 */
	void SaveFramePackage();

	/**
	 * 获取特定时间点的帧包
	 * @param HitCharacter 目标角色
	 * @param HitTime 目标时间点
	 * @return 对应时间点的帧包
	 */	
	FFramePackage GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime);

	/**
	 * 即时射击武器的命中确认
	 * @param Package 帧包
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param HitLocation 击中位置
	 * @return 命中确认结果
	 */
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter,
								   const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);

	/**
	 * 弹道武器的命中确认
	 * @param Package 帧包
	 * @param HitCharacter 被击中的角色
	 * @param TraceStart 射线起始点
	 * @param InitialVelocity 初始速度
	 * @param HitTime 击中时间
	 * @return 命中确认结果
	 */
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	                                             const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/**
	 * 散弹枪的命中确认
	 * @param FramePackages 帧包数组
	 * @param TraceStart 射线起始点
	 * @param HitLocations 击中位置数组
	 * @return 散弹枪命中确认结果
	 */
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages,
	                                                 const FVector_NetQuantize& TraceStart,
	                                                 const TArray<FVector_NetQuantize>& HitLocations);

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;

	/**
	 * 帧历史记录
	 * 存储过去一段时间内角色的状态快照
	 */
	TDoubleLinkedList<FFramePackage> FrameHistory;

	/**
	 * 最大记录时间（秒）
	 * 决定帧历史保留多久的数据
	 */	
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
};
