#include "Types/GameplayTags.h"
#include "GameplayTagsManager.h"

FGameplayTags FGameplayTags::GameplayTags;

void FGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.Action_Dash_Start = Manager.AddNativeGameplayTag(FName("Skill.Dash.Start"));
	GameplayTags.Action_Dash_End = Manager.AddNativeGameplayTag(FName("Skill.Dash.End"));
	GameplayTags.Skill_Area_Shockwave = Manager.AddNativeGameplayTag(FName("Skill.Area.Shockwave"));
	GameplayTags.Skill_DashSlash = Manager.AddNativeGameplayTag(FName("Skill.DashSlash"));
}