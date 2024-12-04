// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UAnnouncement;
class UCharacterOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
	TObjectPtr<UTexture2D> CrossHairCenter;
	TObjectPtr<UTexture2D> CrossHairLeft;
	TObjectPtr<UTexture2D> CrossHairRight;
	TObjectPtr<UTexture2D> CrossHairTop;
	TObjectPtr<UTexture2D> CrossHairBottom;

	float CrossHairSpread;
	FLinearColor CrossHairColor;
};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category="Player States")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;

	UPROPERTY(EditAnywhere, Category="Announcement")
	TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY()
	TObjectPtr<UAnnouncement> Announcement;

	void AddAnnouncement();

protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FVector2d Spread, FLinearColor CrossHairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
