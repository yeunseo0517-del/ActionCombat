// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/EnhanceSkill.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void UEnhanceSkill::ActivateSkill(AActor* Owner)
{
	if (!Owner || GetWorld()->GetTimerManager().IsTimerActive(EnhanceTimer)) return;

	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsCooldownActivate(SkillSlot)) return;

	StartCoolDown(Owner);

	StatusComp->AddStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->AddEnhancedDamage(EnhanceDamage);
	Owner->GetWorldTimerManager().SetTimer(EnhanceTimer, this, &UEnhanceSkill::DeactivateSkill, Duration, false);
}

void UEnhanceSkill::Init(const FSkillEntry& Config)
{
	Super::Init(Config);
	EnhanceDamage = Config.BaseConfig.EnhanceDamage;
}

void UEnhanceSkill::DeactivateSkill()
{
	UStatusComponent* StatusComp = GetStatusComponent(CachedOwner);
	if (!StatusComp) return;

	StatusComp->SkillDurationEnd(SkillSlot);
	StatusComp->RemoveStatus(EStatusType::EST_EnhancedDamage);
	StatusComp->RemoveEnhancedDamage(EnhanceDamage);
}