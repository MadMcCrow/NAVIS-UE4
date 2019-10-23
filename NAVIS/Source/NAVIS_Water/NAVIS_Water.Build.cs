// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NAVIS_Water : ModuleRules
{
    public NAVIS_Water(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/NAVIS_WaterPCH.h";;

        //Public module names that this module uses.
        //In case you would like to add various classes that you're going to use in your game
        //you should add the core,coreuobject and engine dependencies.
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
        PublicDependencyModuleNames.AddRange(new string[] { "NAVIS_CustomMesh" });

        //The path for the header files
        PublicIncludePaths.AddRange(new string[] { "NAVIS_Water/Public" });

        //The path for the source files
        PrivateIncludePaths.AddRange(new string[] { "NAVIS_Water/Private" });
    }
}
