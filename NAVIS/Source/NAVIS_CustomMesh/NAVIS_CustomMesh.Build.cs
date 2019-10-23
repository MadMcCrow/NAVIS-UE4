// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NAVIS_CustomMesh : ModuleRules
{
    public NAVIS_CustomMesh(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/NAVIS_CustomMeshPCH.h";

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject" });
        PrivateDependencyModuleNames.AddRange(new string[] {"Engine", "RHI", "RenderCore" });

        PublicIncludePaths.AddRange(new string[] { "NAVIS_CustomMesh/Public" });
        PrivateIncludePaths.AddRange(new string[] { "NAVIS_CustomMesh/Private" });
    }
}
