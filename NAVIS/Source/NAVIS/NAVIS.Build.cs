// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class NAVIS : ModuleRules
{
	public NAVIS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "NAVIS_Physics" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		    //The path for the header files
        PublicIncludePaths.AddRange(new string[] { "NAVIS/Public" });

        //The path for the source files
        PrivateIncludePaths.AddRange(new string[] { "NAVIS/Private" });
	}
}
