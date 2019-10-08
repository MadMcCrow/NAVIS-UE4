// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class NAVISTarget : TargetRules
{
	public NAVISTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "NAVIS", "NAVIS_Physics", "NAVIS_Water" } );
	}
}
