// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	TObjectPtr<UTexture2D> CrossHairCenter;
	TObjectPtr<UTexture2D> CrossHairLeft;
	TObjectPtr<UTexture2D> CrossHairRight;
	TObjectPtr<UTexture2D> CrossHairTop;
	TObjectPtr<UTexture2D> CrossHairBottom;

	float CrossHairSpread;
};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter,FVector2d Spread);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
