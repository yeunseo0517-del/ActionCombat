// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/RadialShockwave.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/WeaponHolderInterface.h"
#include "Types/GameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

void URadialShockwave::ActivateSkill(AActor* Owner)
{
	if (!Owner) return;
	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsSkillOnCooldown(SkillID)) return;

	StartCoolDown(Owner);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Owner)) CombatInterface->GetCombatComponent()->ExecuteAttack(AttackTag);
	StatusComp->AddStatus(EStatusType::EST_SuperArmor);

	StatusComp->SkillDurationEnd(SlotKey);
}

void URadialShockwave::Init(const FSkillEntry& Config, int32 InSlotKey, int32 InSkillID)
{
	Super::Init(Config, InSlotKey, InSkillID);
	DefaultDamage = Config.SubConfig.ShockConfig.SkillDamage;
}