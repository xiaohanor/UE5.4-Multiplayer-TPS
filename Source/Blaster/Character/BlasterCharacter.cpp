// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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
	OverheadWidget-> SetupAttachment(RootComponent);

	Combat=CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//关闭摄像机和角色的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter,OverlappingWeapon,COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//注册增强输入子系统
	APlayerController* PlayerControl = Cast<APlayerController>(Controller);
	if(PlayerControl)
	{
		UEnhancedInputLocalPlayerSubsystem* LocalSS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControl->GetLocalPlayer());
		if(LocalSS)
		{
			LocalSS->AddMappingContext(PlayerInputMapping,0);
		}
		//设置摄像机视角限制
		PlayerControl->PlayerCameraManager->ViewPitchMax = 400.f;
		PlayerControl->PlayerCameraManager->ViewPitchMin = -40.f;
	}
	
}


void ABlasterCharacter::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
	
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed ==0.f && !bIsInAir) //站立静止不动时
	{
		FRotator CurrentAimRotation = FRotator(0,GetControlRotation().Yaw,0);
		FRotator DeltaAimRotaion = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotaion.Yaw;
		bUseControllerRotationYaw = false;
	}
	if(Speed>0.f || bIsInAir) //跑或跳时
	{
		StartingAimRotation = FRotator(0,GetControlRotation().Yaw,0);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}
	
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch>90.f && !IsLocallyControlled())
	{
		//将Pitch值从[270,360)映射到[-90,0)之间
		FVector2D InRange(270.f,360.f);
		FVector2D OutRange(-90.f,0.f);
		AO_Pitch=FMath::GetMappedRangeValueClamped(InRange,OutRange,AO_Pitch);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}
	
	OverlappingWeapon = Weapon;
	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
	
}

//当OverlappingWeapon更新时调用
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
	
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2d F2d = Value.Get<FVector2d>();
	if(Controller)
	{
		const FRotator Ro = Controller->GetControlRotation();
		const FRotator YawRo(0,Ro.Yaw,0);
		const FVector PlayerF = FRotationMatrix(YawRo).GetUnitAxis(EAxis::X);
		const FVector PlayerR = FRotationMatrix(YawRo).GetUnitAxis(EAxis::Y);
		AddMovementInput(PlayerF,F2d.Y);
		AddMovementInput(PlayerR,F2d.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d F2d = Value.Get<FVector2d>();
	if(Controller)
	{
		AddControllerPitchInput(F2d.Y);
		AddControllerYawInput(F2d.X);
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if(Combat)
	{
		if(HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);

		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* PlayerInputC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInputC)
	{
		PlayerInputC->BindAction(PlayerMove,ETriggerEvent::Triggered,this,&ABlasterCharacter::Move);
		PlayerInputC->BindAction(PlayerLook,ETriggerEvent::Triggered,this,&ABlasterCharacter::Look);
		PlayerInputC->BindAction(PlayerJump,ETriggerEvent::Started,this,&ABlasterCharacter::Jump);
		PlayerInputC->BindAction(Equip,ETriggerEvent::Triggered,this,&ABlasterCharacter::EquipButtonPressed);
		PlayerInputC->BindAction(PlayerCrouch,ETriggerEvent::Started,this,&ABlasterCharacter::CrouchButtonPressed);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Started,this,&ABlasterCharacter::AimButtonPressed);
		PlayerInputC->BindAction(PlayerAim,ETriggerEvent::Completed,this,&ABlasterCharacter::AimButtonReleased);

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
	if(Combat==nullptr) return nullptr;
	return Combat->EquippedWeapon;
}



