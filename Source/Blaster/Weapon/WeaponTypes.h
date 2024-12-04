﻿#pragma once

#define TRACE_LENGTH 8000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_ShotGun UMETA(DisplayName = "ShotGun"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};
