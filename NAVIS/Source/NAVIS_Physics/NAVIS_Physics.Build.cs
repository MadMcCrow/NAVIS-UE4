// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NAVIS_Physics : ModuleRules
{
    public NAVIS_Physics(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/NAVIS_PhysicsPCH.h";

        //PrivateDependencyModuleNames.Add("NAVIS");
        //PublicDependencyModuleNames.Add("NAVIS");

        //Public module names that this module uses.
        //In case you would like to add various classes that you're going to use in your game
        //you should add the core,coreuobject and engine dependencies.
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
        PublicDependencyModuleNames.AddRange(new string[] { "PhysX", "APEX"});
        //PublicDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay" });
        //PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");

        //The path for the header files
        PublicIncludePaths.AddRange(new string[] { "NAVIS_Physics/Public" });

        //The path for the source files
        PrivateIncludePaths.AddRange(new string[] { "NAVIS_Physics/Private" });


        // Uncomment if you are using online features
        //PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        //DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
    }
}
