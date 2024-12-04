// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"


void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2d ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = HUDPackage.CrossHairSpread * CrosshairSpreadMax;

		if (HUDPackage.CrossHairCenter)
		{
			FVector2d Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrossHairCenter, ViewportCenter, Spread, HUDPackage.CrossHairColor);
		}
		if (HUDPackage.CrossHairLeft)
		{
			FVector2d Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrossHairLeft, ViewportCenter, Spread, HUDPackage.CrossHairColor);
		}
		if (HUDPackage.CrossHairRight)
		{
			FVector2d Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrossHairRight, ViewportCenter, Spread, HUDPackage.CrossHairColor);
		}
		if (HUDPackage.CrossHairTop)
		{
			FVector2d Spread(0.f, -SpreadScaled); //UV坐标系Y轴向下
			DrawCrosshair(HUDPackage.CrossHairTop, ViewportCenter, Spread, HUDPackage.CrossHairColor);
		}
		if (HUDPackage.CrossHairBottom)
		{
			FVector2d Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrossHairBottom, ViewportCenter, Spread, HUDPackage.CrossHairColor);
		}
	}
}

void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FVector2d Spread,
                                FLinearColor CrossHairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2d TextureDrawPoint(
		(ViewportCenter.X - TextureWidth / 2.f) + Spread.X,
		(ViewportCenter.Y - TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrossHairColor
	);
}
