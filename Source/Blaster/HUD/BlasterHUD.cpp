// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncement.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"


void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (IsValid(GEngine))
	{
		if (IsValid(GEngine->GameViewport))
		{
			FVector2d ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2d ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

			float SpreadScaled = HUDPackage.CrossHairSpread * CrosshairSpreadMax;
			if (IsValid(HUDPackage.CrossHairCenter))
			{
				FVector2d Spread(0.f, 0.f);
				DrawCrosshair(HUDPackage.CrossHairCenter, ViewportCenter, Spread, HUDPackage.CrossHairColor);
			}
			if (IsValid(HUDPackage.CrossHairLeft))
			{
				FVector2d Spread(-SpreadScaled, 0.f);
				DrawCrosshair(HUDPackage.CrossHairLeft, ViewportCenter, Spread, HUDPackage.CrossHairColor);
			}
			if (IsValid(HUDPackage.CrossHairRight))
			{
				FVector2d Spread(SpreadScaled, 0.f);
				DrawCrosshair(HUDPackage.CrossHairRight, ViewportCenter, Spread, HUDPackage.CrossHairColor);
			}
			if (IsValid(HUDPackage.CrossHairTop))
			{
				FVector2d Spread(0.f, -SpreadScaled); //UV坐标系Y轴向下
				DrawCrosshair(HUDPackage.CrossHairTop, ViewportCenter, Spread, HUDPackage.CrossHairColor);
			}
			if (IsValid(HUDPackage.CrossHairBottom))
			{
				FVector2d Spread(0.f, SpreadScaled);
				DrawCrosshair(HUDPackage.CrossHairBottom, ViewportCenter, Spread, HUDPackage.CrossHairColor);
			}
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

void ABlasterHUD::AddElimAnnouncement(const FString& AttackerName, const FString& VictimName)
{
	OwningPlayerController = OwningPlayerController == nullptr
		                         ? TObjectPtr<APlayerController>(GetOwningPlayerController())
		                         : OwningPlayerController;
	if (OwningPlayerController && ElimAnnouncementClass)
	{
		TObjectPtr<UElimAnnouncement> ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayerController,
			ElimAnnouncementClass);
		if (ElimAnnouncementWidget)
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(AttackerName, VictimName);
			ElimAnnouncementWidget->AddToViewport();

			// 将已有的淘汰消息向上移动
			for (UElimAnnouncement* Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* Slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (Slot)
					{
						FVector2d Position = Slot->GetPosition();
						// 向上移动一个消息的高度
						FVector2d NewPosition(
							Slot->GetPosition().X,
							Position.Y - Slot->GetSize().Y
						);
						Slot->SetPosition(NewPosition);
					}
				}
			}

			ElimMessages.Add(ElimAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgTimerDelegate;
			ElimMsgTimerDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgTimerDelegate,
				ElimAnnouncementTime,
				false
			);
		}
	}
}

void ABlasterHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FVector2d Spread,
                                FLinearColor CrossHairColor)
{
	if (!IsValid(Texture)) return;

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
