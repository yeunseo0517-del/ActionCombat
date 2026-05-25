#include "Types/GameplayTags.h"
#include "GameplayTagsManager.h"

FGameplayTags FGameplayTags::GameplayTags;

void FGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	GameplayTags.Action_Attack_Basic = Manager.AddNativeGameplayTag(FName("Action.Attack.Basic"));
	GameplayTags.Action_Movement_Dash = Manager.AddNativeGameplayTag(FName("Action.Movement.Dash"));
	GameplayTags.Action_Movement_Dash_Start = Manager.AddNativeGameplayTag(FName("Action.Movement.Dash.Start"));
	GameplayTags.Action_Movement_Dash_End = Manager.AddNativeGameplayTag(FName("Action.Movement.Dash.End"));

	GameplayTags.Skill_EnhanceDamage = Manager.AddNativeGameplayTag(FName("Skill.EnhanceDamage"));
	GameplayTags.Skill_DashSlash = Manager.AddNativeGameplayTag(FName("Skill.DashSlash"));
	GameplayTags.Skill_Shockwave = Manager.AddNativeGameplayTag(FName("Skill.Shockwave"));
	GameplayTags.Skill_Projectile = Manager.AddNativeGameplayTag(FName("Skill.Projectile"));
	GameplayTags.Skill_Projectile_FireBall = Manager.AddNativeGameplayTag(FName("Skill.Projectile.FireBall"));
	GameplayTags.Skill_Projectile_GroundCrash = Manager.AddNativeGameplayTag(FName("Skill.Projectile.GroundCrash"));

	GameplayTags.Type_Melee = Manager.AddNativeGameplayTag(FName("Type.Melee"));
	GameplayTags.Type_Area = Manager.AddNativeGameplayTag(FName("Type.Area"));

	GameplayTags.Effect_Invincible = Manager.AddNativeGameplayTag(FName("Effect.Invincible"));
	GameplayTags.Effect_SuperArmor = Manager.AddNativeGameplayTag(FName("Effect.SuperArmor"));

	GameplayTags.State_Common_Attacking = Manager.AddNativeGameplayTag(FName("State.Attacking"));
	GameplayTags.State_Common_HitReact = Manager.AddNativeGameplayTag(FName("State.Common.HitReact"));
	GameplayTags.State_Common_Dead = Manager.AddNativeGameplayTag(FName("State.Common.Dead"));
	GameplayTags.State_Player_Equip = Manager.AddNativeGameplayTag(FName("State.Player.Equip"));
	GameplayTags.State_AI_Patrolling = Manager.AddNativeGameplayTag(FName("State.AI.Patrolling"));
	GameplayTags.State_AI_Chasing = Manager.AddNativeGameplayTag(FName("State.AI.Chasing"));
	GameplayTags.State_AI_Engaged = Manager.AddNativeGameplayTag(FName("State.AI.Engaged"));
}