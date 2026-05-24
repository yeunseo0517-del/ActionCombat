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

	StatusComp->ActivateSkill(SlotKey, SkillID, SkillData.BaseConfig.Cooldown);
}

FHitContext USkillBase::GetSkillHitContext()
{
	if (!CachedOwner) return FHitContext();
	FHitContext HitContext;
	HitContext.Instigator = CachedOwner;
	HitContext.DamageCauser = CachedOwner;
	HitContext.AttackTag = AttackTag;
	HitContext.Damage = DefaultDamage;
	return HitContext;
}

void USkillBase::Init(const FSkillEntry& Config, int32 InSlotKey, int32 InSkillID)
{
	SkillData = Config;
	SlotKey = InSlotKey;
	SkillID = InSkillID;
}

bool USkillBase::IsSkillOnCooldown() const
{
	if (!CachedOwner) return false;
	UStatusComponent* StatusComp = GetStatusComponent(CachedOwner);
	if (!StatusComp) return false;
	return StatusComp->IsSkillOnCooldown(SkillID);
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
