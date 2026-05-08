// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionCombact.h"
#include "Modules/ModuleManager.h"
#include "Types/GameplayTags.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FActionCombactModule, ActionCombact, "ActionCombact" );

void FActionCombactModule::StartupModule()
{
	FGameplayTags::InitializeNativeTags();
}

void FActionCombactModule::ShutdownModule()
{
}
