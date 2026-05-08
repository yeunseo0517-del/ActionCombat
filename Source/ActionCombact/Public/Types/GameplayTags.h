#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FGameplayTags
{
public:
	static const FGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	FGameplayTag Action_Attack_Basic;
	FGameplayTag Action_Movement_Dash;
	FGameplayTag Action_Movement_Dash_Start;
	FGameplayTag Action_Movement_Dash_End;

	FGameplayTag Skill_DashSlash;
	FGameplayTag Skill_Shockwave;
	FGameplayTag Skill_EnhanceDamage;

	FGameplayTag Type_Melee;
	FGameplayTag Type_Area;

	FGameplayTag State_Common_Attacking;
	FGameplayTag State_Common_HitReact;
	FGameplayTag State_Common_Dead;
	FGameplayTag State_Player_Equip;
	FGameplayTag State_AI_Patrolling;
	FGameplayTag State_AI_Chasing;
	FGameplayTag State_AI_Engaged;


private:
	static FGameplayTags GameplayTags;
};