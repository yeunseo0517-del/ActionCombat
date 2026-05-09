// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/SkillBase.h"
#include "Items/Weapons/Weapon.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void USkillBase::StartCoolDown(AActor* Owner)
{
	if (!Owner) return;
	CachedOwner = Owner;

	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp) return;

	StatusComp->ActivateSkill(SkillData.SkillSlot, SkillData.BaseConfig.Cooldown);
}

void USkillBase::Init(const FSkillEntry& Config)
{
	SkillData = Config;
}

UStatusComponent* USkillBase::GetStatusComponent(AActor* Owner) const
{
	if (!Owner) return nullptr;

	if (IStatusReceiverInterface* Receiver = Cast<IStatusReceiverInterface>(Owner))
	{
		return Receiver->GetStatusComponent();
	}
	return nullptr;
}
