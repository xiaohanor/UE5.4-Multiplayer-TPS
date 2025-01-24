// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncement.h"

#include "Components/TextBlock.h"

void UElimAnnouncement::SetElimAnnouncementText(const FString& AttackerName, const FString& VictimName)
{
	FText Announcement = FText::Format(NSLOCTEXT("Game", "ElimAnnouncement", "{0} 淘汰了 {1}"), FText::FromString(AttackerName), FText::FromString(VictimName));
	AnnouncementText->SetText(Announcement);
}
