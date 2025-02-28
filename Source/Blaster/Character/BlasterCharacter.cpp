// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Blaster.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerStart/TeamPlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapon/Flag.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensationComponent"));

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//关闭摄像机和角色的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0, 840.f, 0);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	BaseSpeed = 800.f;
	SprintSpeed = 1200.f;

	/*
	 * 用于服务器端倒带的 HitBox
	 */

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_Hitbox);
			Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_Hitbox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->InitializeSpeed(BaseSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);	//初始化速度，第一个参数是BaseSpeed是因为当前角色可能处在奔跑状态
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::SpawnDefaultWeapon()
{
	BlasterGameMode = BlasterGameMode == nullptr ? TObjectPtr<ABlasterGameMode>(GetWorld()->GetAuthGameMode<ABlasterGameMode>()): BlasterGameMode;
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StaringWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StaringWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StaringWeapon);
		}
	}
}

void ABlasterCharacter::MulticastGainedCrown_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ABlasterCharacter::MulticastLostCrown_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ABlasterCharacter::SetTeamColor(ETeam Team)
{
	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatIns;
		break;
	case ETeam::ET_TeamBlue:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatIns;
		break;
	case ETeam::ET_TeamRed:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatIns;
		break;
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	if (BlasterPlayerController)
	{
		//注册增强输入子系统
		UEnhancedInputLocalPlayerSubsystem* LocalSS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			BlasterPlayerController->GetLocalPlayer());
		if (LocalSS)
		{
			LocalSS->AddMappingContext(PlayerInputMapping, 0);
		}

		//设置摄像机视角限制
		BlasterPlayerController->PlayerCameraManager->ViewPitchMax = 400.f;
		BlasterPlayerController->PlayerCameraManager->ViewPitchMin = -40.f;

		//设置角色移动速度等于自定义的基础速度
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	}
				
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	UpdateHUDGrenades();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (Combat && Combat->bHoldingTheFlag)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (Combat->EquippedWeapon)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	//如果是服务器或者本地控制的角色
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::OnPlayerStateInitialized()
{
	BlasterPlayerState->AddToScore(0.f);
	BlasterPlayerState->AddToDefeats(0);
	SetTeamColor(BlasterPlayerState->GetTeam());
	SetSpawnPoint();
}

void ABlasterCharacter::SetSpawnPoint()
{
	if (HasAuthority() && GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		if (TeamPlayerStarts.Num() > 0)
		{
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			SetActorLocationAndRotation(ChosenPlayerStart->GetActorLocation(), ChosenPlayerStart->GetActorRotation());
		}
	}
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//将Pitch值从[270,360)映射到[-90,0)之间
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) //站立静止不动时
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		FRotator DeltaAimRotaion = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotaion.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //跑或跳时
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 6.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0, GetBaseAimRotation().Yaw, 0);
		}
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;


	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			OnPlayerStateInitialized();

			ABlasterGameState* BlasterGameState = GetWorld()->GetGameState<ABlasterGameState>();
			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(BlasterPlayerState))
			{
				MulticastGainedCrown();
			}
		}
	}

	if (BlasterPlayerController == nullptr)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr
			                          ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Controller))
			                          : BlasterPlayerController;
		if (BlasterPlayerController)
		{
			//注册增强输入子系统
			UEnhancedInputLocalPlayerSubsystem* LocalSS = ULocalPlayer::GetSubsystem<
				UEnhancedInputLocalPlayerSubsystem>(
				BlasterPlayerController->GetLocalPlayer());
			if (LocalSS)
			{
				LocalSS->AddMappingContext(PlayerInputMapping, 0);
			}

			//设置摄像机视角限制
			BlasterPlayerController->PlayerCameraManager->ViewPitchMax = 400.f;
			BlasterPlayerController->PlayerCameraManager->ViewPitchMin = -40.f;
		}
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatorController, AActor* DamageCauser)
{
	BlasterGameMode = BlasterGameMode == nullptr ? TObjectPtr<ABlasterGameMode>(GetWorld()->GetAuthGameMode<ABlasterGameMode>()): BlasterGameMode;
	if (bElimmed || BlasterGameMode == nullptr) return;
	Damage = BlasterGameMode->CalculateDamage(InstigatorController, Controller, Damage);
	
	float DamageToHealth = Damage;
	if (Shield >= DamageToHealth)
	{
		Shield = FMath::Clamp(Shield - DamageToHealth, 0.f, MaxShield);
		DamageToHealth = 0.f;
	}
	else
	{
		DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
		Shield = 0.f;
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
	BlasterGameMode = BlasterGameMode == nullptr ? TObjectPtr<ABlasterGameMode>(GetWorld()->GetAuthGameMode<ABlasterGameMode>()): BlasterGameMode;
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr
				                          ? TObjectPtr<ABlasterPlayerController>(
					                          Cast<ABlasterPlayerController>(Controller))
				                          : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr
		                          ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Controller))
		                          : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr
		                          ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Controller))
		                          : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr
								  ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Controller))
								  : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void ABlasterCharacter::UpdateHUDGrenades()
{
	BlasterPlayerController = BlasterPlayerController == nullptr
								  ? TObjectPtr<ABlasterPlayerController>(Cast<ABlasterPlayerController>(Controller))
								  : BlasterPlayerController;
	if (BlasterPlayerController && Combat)
	{
		BlasterPlayerController->SetHUDGrenades(Combat->Grenades);
	}
}

void ABlasterCharacter::Elim(bool bPlayerLeft)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeft);
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		if (Combat->TheFlag)
		{
			DropOrDestroyWeapon(Combat->TheFlag);
		}
	}
}

void ABlasterCharacter::MulticastElim_Implementation(bool bPlayerLeft)
{
	bLeftGame = bPlayerLeft;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	//开始溶解效果
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), -0.5f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 400.f);
	}
	StartDissolve();

	//禁用角色移动
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	//禁用碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//生成 Elim Bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}

	//关闭狙击枪瞄准镜画面
	if (IsLocallyControlled())
	{
		bool bHideSniperScope = Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->
			GetWeaponType() == EWeaponType::EWT_SniperRifle;
		if (bHideSniperScope)
		{
			ShowSniperScope(false);
			MouseSensitivity = 1.f;
		}
	}

	//如果角色拥有王冠，丢弃王冠
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ThisClass::ElimTimerFinished,
		ElimDelay
	);
}

void ABlasterCharacter::ElimTimerFinished()
{
	BlasterGameMode = BlasterGameMode == nullptr ? TObjectPtr<ABlasterGameMode>(GetWorld()->GetAuthGameMode<ABlasterGameMode>()): BlasterGameMode;
	if (BlasterGameMode && !bLeftGame)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

void ABlasterCharacter::ServerLeaveGame_Implementation()
{
	BlasterGameMode = BlasterGameMode == nullptr ? TObjectPtr<ABlasterGameMode>(GetWorld()->GetAuthGameMode<ABlasterGameMode>()): BlasterGameMode;
	BlasterPlayerState = BlasterPlayerState == nullptr ? TObjectPtr<ABlasterPlayerState>(GetPlayerState<ABlasterPlayerState>()) : BlasterPlayerState;
	if (BlasterGameMode && BlasterPlayerState)
	{
		BlasterGameMode->PlayerLeftGame(BlasterPlayerState);
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
		UE_LOG(LogTemp, Warning, TEXT("Play Reload Montage, CharacterRole: %s"),
		       *UEnum::GetValueAsString(GetLocalRole()));
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlaySwapWeaponMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapWeaponMontage)
	{
		AnimInstance->Montage_Play(SwapWeaponMontage);
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	return Velocity.Size();
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	//如果生命值减少，播放受击动画
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	//如果护盾值减少，播放受击动画
	if (Health < LastShield)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
	if (DissolveCurve)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}

//当OverlappingWeapon更新时调用
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled())
	{
		return;
	}
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->WeaponMeshGetter())
		{
			Combat->EquippedWeapon->WeaponMeshGetter()->bOwnerNoSee = true;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->WeaponMeshGetter())
		{
			Combat->SecondaryWeapon->WeaponMeshGetter()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->WeaponMeshGetter())
		{
			Combat->EquippedWeapon->WeaponMeshGetter()->bOwnerNoSee = false;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->WeaponMeshGetter())
		{
			Combat->SecondaryWeapon->WeaponMeshGetter()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	if (bDisableGameplay)
	{
		return;
	}

	const FVector2d F2d = Value.Get<FVector2d>();
	if (Controller)
	{
		const FRotator Ro = Controller->GetControlRotation();
		const FRotator YawRo(0, Ro.Yaw, 0);
		const FVector PlayerF = FRotationMatrix(YawRo).GetUnitAxis(EAxis::X);
		const FVector PlayerR = FRotationMatrix(YawRo).GetUnitAxis(EAxis::Y);
		AddMovementInput(PlayerF, F2d.Y);
		AddMovementInput(PlayerR, F2d.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d F2d = Value.Get<FVector2d>();
	if (Controller)
	{
		AddControllerPitchInput(F2d.Y * MouseSensitivity);
		AddControllerYawInput(F2d.X * MouseSensitivity);
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;

	if (Combat)
	{
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		bool bSwap = Combat->ShouldSwapWeapons() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied && OverlappingWeapon == nullptr;
		if (bSwap)
		{
			PlaySwapWeaponMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			Combat->SwapWeapons();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::SprintButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	GetCharacterMovement()->MaxWalkSpeed += SprintSpeed;
}

void ABlasterCharacter::SprintButtonReleased()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	GetCharacterMovement()->MaxWalkSpeed -= SprintSpeed;
}

void ABlasterCharacter::AimButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::ThrowGrenadeButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;

	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void ABlasterCharacter::Jump()
{
	if (bDisableGameplay)
	{
		return;
	}
	if (Combat && Combat->bHoldingTheFlag) return;


	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* PlayerInputC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInputC)
	{
		PlayerInputC->BindAction(PlayerMove, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);
		PlayerInputC->BindAction(PlayerLook, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
		PlayerInputC->BindAction(PlayerJump, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		PlayerInputC->BindAction(Equip, ETriggerEvent::Started, this, &ABlasterCharacter::EquipButtonPressed);
		PlayerInputC->BindAction(PlayerCrouch, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);
		PlayerInputC->BindAction(PlayerAim, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
		PlayerInputC->BindAction(PlayerAim, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);
		PlayerInputC->BindAction(PlayerSprint, ETriggerEvent::Started, this, &ABlasterCharacter::SprintButtonPressed);
		PlayerInputC->BindAction(PlayerSprint, ETriggerEvent::Completed, this,
		                         &ABlasterCharacter::SprintButtonReleased);
		PlayerInputC->BindAction(PlayerFire, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
		PlayerInputC->BindAction(PlayerFire, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);
		PlayerInputC->BindAction(Reload, ETriggerEvent::Started, this, &ABlasterCharacter::ReloadButtonPressed);
		PlayerInputC->BindAction(ThrowGrenade, ETriggerEvent::Started, this,
		                         &ABlasterCharacter::ThrowGrenadeButtonPressed);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)
	{
		return nullptr;
	}
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr)
	{
		return FVector();
	}
	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr)
	{
		return ECombatState::ECS_MAX;
	}
	return Combat->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading() const
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

bool ABlasterCharacter::IsHoldingTheFlag() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingTheFlag;
}

ETeam ABlasterCharacter::GetTeam()
{
	BlasterPlayerState = BlasterPlayerState == nullptr ? TObjectPtr<ABlasterPlayerState>(GetPlayerState<ABlasterPlayerState>()) : BlasterPlayerState;
	if (BlasterPlayerState == nullptr)
	{
		return ETeam::ET_NoTeam;
	}
	return BlasterPlayerState->GetTeam();
}

void ABlasterCharacter::SetHoldingTheFlag(bool bHold)
{
	if (Combat == nullptr) return;
	Combat->bHoldingTheFlag = bHold;
}

void ABlasterCharacter::SetTheFlag(bool bHold)
{
	if (Combat == nullptr) return;
	Combat->TheFlag = bHold ? Combat->TheFlag : nullptr;
}
