// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blaster/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter,OverlappingWeapon,COND_OwnerOnly);
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

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* PlayerInputC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInputC)
	{
		PlayerInputC->BindAction(PlayerMove,ETriggerEvent::Triggered,this,&ABlasterCharacter::Move);
		PlayerInputC->BindAction(PlayerLook,ETriggerEvent::Triggered,this,&ABlasterCharacter::Look);
		PlayerInputC->BindAction(PlayerJump,ETriggerEvent::Started,this,&ABlasterCharacter::Jump);
	}

}



