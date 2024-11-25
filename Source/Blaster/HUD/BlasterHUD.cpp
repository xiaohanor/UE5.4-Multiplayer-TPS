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

		float SpreadScaled = HUDPackage.CrossHairSpread * CrosshairSpreadMax;
		
		if(HUDPackage.CrossHairCenter)
		{
			FVector2d Spread(0.f,.0f);
			DrawCrosshair(HUDPackage.CrossHairCenter, ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairLeft)
		{
			FVector2d Spread(-SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrossHairLeft, ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairRight)
		{
			FVector2d Spread(SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrossHairRight, ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairTop)
		{
			FVector2d Spread(0.f,-SpreadScaled);	//UV坐标系Y轴向下
			DrawCrosshair(HUDPackage.CrossHairTop, ViewportCenter,Spread);
		}
		if(HUDPackage.CrossHairBottom)
		{
			FVector2d Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrossHairBottom, ViewportCenter,Spread);
		}
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter,FVector2d Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2d TextureDrawPoint(
		(ViewportCenter.X - TextureWidth / 2.f)+Spread.X,
		(ViewportCenter.Y - TextureHeight / 2.f)+Spread.Y
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
		FLinearColor::White
		);
	
}
