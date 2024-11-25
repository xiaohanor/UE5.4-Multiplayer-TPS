// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"


void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2d ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if(HUDPackage.CrossHairCenter)
		{
			DrawCrosshair(HUDPackage.CrossHairCenter, ViewportCenter);
		}
		if(HUDPackage.CrossHairLeft)
		{
			DrawCrosshair(HUDPackage.CrossHairLeft, ViewportCenter);
		}
		if(HUDPackage.CrossHairRight)
		{
			DrawCrosshair(HUDPackage.CrossHairRight, ViewportCenter);
		}
		if(HUDPackage.CrossHairTop)
		{
			DrawCrosshair(HUDPackage.CrossHairTop, ViewportCenter);
		}
		if(HUDPackage.CrossHairBottom)
		{
			DrawCrosshair(HUDPackage.CrossHairBottom, ViewportCenter);
		}
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2d TextureDrawPoint(ViewportCenter.X - TextureWidth / 2.f, ViewportCenter.Y - TextureHeight / 2.f);

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
		FLinearColor::White
		);

	UE_LOG(LogTemp, Warning, TEXT("DrawHUD"));

}
