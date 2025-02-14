// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RogueLikeEditorTarget : TargetRules
{
	public RogueLikeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("RogueLike");
	}
}