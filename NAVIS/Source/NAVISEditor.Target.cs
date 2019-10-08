// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class NAVISEditorTarget : TargetRules
{
	public NAVISEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "NAVIS", "NAVIS_Physics", "NAVIS_Water" } );
	}
}
