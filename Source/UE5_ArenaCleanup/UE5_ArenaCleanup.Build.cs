// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_ArenaCleanup : ModuleRules
{
	public UE5_ArenaCleanup(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UE5_ArenaCleanup",
			"UE5_ArenaCleanup/Variant_Horror",
			"UE5_ArenaCleanup/Variant_Horror/UI",
			"UE5_ArenaCleanup/Variant_Shooter",
			"UE5_ArenaCleanup/Variant_Shooter/AI",
			"UE5_ArenaCleanup/Variant_Shooter/UI",
			"UE5_ArenaCleanup/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
