// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/ProjectileSkill.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Interfaces/CombatInterface.h"

void UProjectileSkill::ActivateSkill(AActor* Owner)
{
	if (!Owner) return;
	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsSkillOnCooldown(SkillID)) return;

	StartCoolDown(Owner);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Owner)) CombatInterface->GetCombatComponent()->ExecuteAttack(FGameplayTags::Get().Skill_Projectile);
	StatusComp->SkillDurationEnd(SlotKey);
}

void UProjectileSkill::Init(const FSkillEntry& Config, int32 InSlotKey, int32 InSkillID)
{
	Super::Init(Config, InSlotKey, InSkillID);
	DefaultDamage = Config.SubConfig.ProjectileConfig.SkillDamage;
}