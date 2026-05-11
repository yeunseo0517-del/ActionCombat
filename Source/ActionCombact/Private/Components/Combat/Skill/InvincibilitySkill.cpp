// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Skill/InvincibilitySkill.h"
#include "Interfaces/CombatInterface.h"
#include "Types/CombatTypes.h"
#include "Types/GameplayTags.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"

void UInvincibilitySkill::ActivateSkill(AActor* Owner)
{
	if (!Owner) return;
	UStatusComponent* StatusComp = GetStatusComponent(Owner);
	if (!StatusComp || StatusComp->IsSkillOnCooldown(SkillID)) return;
	UE_LOG(LogTemp, Warning, TEXT("Start Dash"))
	StartCoolDown(Owner);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Owner)) CombatInterface->GetCombatComponent()->ExecuteAttack(AttackTag);
	else UE_LOG(LogTemp, Error, TEXT("Fail to Cast Interface"));

	StatusComp->SkillDurationEnd(SlotKey);
}

void UInvincibilitySkill::Init(const FSkillEntry& Config, int32 InSlotKey, int32 InSkillID)
{
	Super::Init(Config, InSlotKey, InSkillID);
	AttackTag = FGameplayTags::Get().Skill_DashSlash;
}
