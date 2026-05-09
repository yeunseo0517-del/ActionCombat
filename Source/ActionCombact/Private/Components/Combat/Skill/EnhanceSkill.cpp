// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/EnhanceSkill.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void UEnhanceSkill::ActivateSkill(AActor* Owner)
{
	if (!Owner || GetWorld()->GetTimerManager().IsTimerActive(EnhanceTimer)) return;

	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsCooldownActivate(SkillData.SkillSlot)) return;

	StartCoolDown(Owner);

	StatusComp->AddStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->AddEnhancedDamage(SkillData.BaseConfig.EnhanceDamage);
	Owner->GetWorldTimerManager().SetTimer(EnhanceTimer, this, &UEnhanceSkill::DeactivateSkill, SkillData.BaseConfig.Duration, false);
}

void UEnhanceSkill::Init(const FSkillEntry& Config)
{
	Super::Init(Config);
	AttackTag = FGameplayTags::Get().Skill_EnhanceDamage;
}

void UEnhanceSkill::DeactivateSkill()
{
	UStatusComponent* StatusComp = GetStatusComponent(CachedOwner);
	if (!StatusComp) return;

	StatusComp->SkillDurationEnd(SkillData.SkillSlot);
	StatusComp->RemoveStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->RemoveEnhancedDamage(SkillData.BaseConfig.EnhanceDamage);
}