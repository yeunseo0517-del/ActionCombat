// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/EnhanceSkill.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void UEnhanceSkill::ActivateSkill(AActor* Owner)
{
	if (!Owner || GetWorld()->GetTimerManager().IsTimerActive(EnhanceTimer)) return;

	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsSkillOnCooldown(SkillID)) return;

	StartCoolDown(Owner);

	StatusComp->AddStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->AddEnhancedDamage(SkillData.BaseConfig.EnhanceConfig.SkillDamage);
	Owner->GetWorldTimerManager().SetTimer(EnhanceTimer, this, &UEnhanceSkill::DeactivateSkill, SkillData.BaseConfig.EnhanceConfig.Duration, false);
}

void UEnhanceSkill::Init(const FSkillEntry& Config, int32 InSlotKey, int32 InSkillID)
{
	Super::Init(Config, InSlotKey, InSkillID);
	AttackTag = FGameplayTags::Get().Skill_EnhanceDamage;
	DefaultDamage = Config.BaseConfig.EnhanceConfig.SkillDamage;
}

void UEnhanceSkill::DeactivateSkill()
{
	UStatusComponent* StatusComp = GetStatusComponent(CachedOwner);
	if (!StatusComp) return;

	StatusComp->SkillDurationEnd(SlotKey);
	StatusComp->RemoveStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->RemoveEnhancedDamage(SkillData.BaseConfig.EnhanceConfig.SkillDamage);
}