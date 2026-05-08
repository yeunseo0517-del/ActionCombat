#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FGameplayTags
{
public:
	static const FGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	FGameplayTag Skill_Area_Shockwave;
	FGameplayTag Skill_DashSlash;

	FGameplayTag Action_Dash_Start;
	FGameplayTag Action_Dash_End;

private:
	static FGameplayTags GameplayTags;
};