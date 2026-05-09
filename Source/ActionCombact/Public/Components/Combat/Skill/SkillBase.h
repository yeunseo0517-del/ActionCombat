// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/SkillTypes.h"
#include "Types/GameplayTags.h"
#include "NiagaraComponent.h"
#include "SkillBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class ACTIONCOMBACT_API USkillBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void ActivateSkill(AActor* Owner) PURE_VIRTUAL(USkillBase::ActivateSkill, );
	virtual void Init(const FSkillEntry& Config);
	

protected:
	class UStatusComponent* GetStatusComponent(AActor* Owner) const;
	void StartCoolDown(AActor* Owner);

	UPROPERTY()
	AActor* CachedOwner;

	FSkillEntry SkillData;
	FGameplayTag AttackTag;

public:
	virtual const FGameplayTag& GetTag() { return AttackTag; }
	float GetSkillDamage() const { return SkillData.BaseConfig.SkillDamage; }
	float GetDistance() const { return SkillData.BaseConfig.DashDistance; }
	float GetDuration() const { return SkillData.BaseConfig.Duration; }
	float GetMaxRadius() const { return SkillData.BaseConfig.MaxRadius; }
	UNiagaraSystem* GetEffect() const { return SkillData.BaseConfig.Niagara; }
};
