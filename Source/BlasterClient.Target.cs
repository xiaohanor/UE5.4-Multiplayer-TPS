// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BlasterClientTarget : TargetRules
{
	public BlasterClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("Blaster");
		ExtraModuleNames.Add("DedicatedServers");
	}
}