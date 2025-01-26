#pragma once

UENUM()
enum class ETeam : uint8
{
	ET_TeamRed UMETA(DisplayName = "RedTeam"),
	ET_TeamBlue UMETA(DisplayName = "BlueTeam"),
	ET_NoTeam UMETA(DisplayName = "NoTeam"),
	
	ET_MAX UMETA(DisplayName = "DefaultMAX")
};
