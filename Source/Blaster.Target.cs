// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlasterTarget : TargetRules
{
	public BlasterTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("Blaster");
		ExtraModuleNames.Add("DedicatedServers");
	}
}