// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlasterServerTarget : TargetRules
{
	public BlasterServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("Blaster");
		ExtraModuleNames.Add("DedicatedServers");
	}
}