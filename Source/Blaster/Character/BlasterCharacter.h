// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "BlasterCharacter.generated.h"

class AWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//玩家输入映射
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInputMappingContext> PlayerInputMapping;

	//玩家输入Action
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerMove;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerLook;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PlayerInput",meta=(AllowPrivateAccess="true"))
	TObjectPtr <UInputAction> PlayerJump;

	//玩家输入函数
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	
	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);


};
