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
	if (!StatusComp || StatusComp->IsCooldownActivate(SkillSlot)) return;

	StartCoolDown(Owner);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Owner)) CombatInterface->GetCombatComponent()->ExecuteAttack(FGameplayTags::Get().Skill_Shockwave);
	else UE_LOG(LogTemp, Error, TEXT("Fail to Cast Interface"));
	StatusComp->AddStatus(EStatusType::EST_SuperArmor);

	StatusComp->SkillDurationEnd(SkillSlot);
}

void URadialShockwave::Init(const FSkillEntry& Config)
{
	Super::Init(Config);
	MaxRadius = Config.BaseConfig.MaxRadius;
	AttackTag = FGameplayTags::Get().Skill_Shockwave;
	if(Config.BaseConfig.Niagara)
		Effect = Config.BaseConfig.Niagara;
}