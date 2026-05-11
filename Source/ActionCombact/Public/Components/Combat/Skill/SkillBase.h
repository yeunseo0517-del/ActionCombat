// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/SkillTypes.h"
#include "Types/GameplayTags.h"
#include "Types/HitContext.h"
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
	virtual void Init(const FSkillEntry& Config, int32 SlotKey, int32 SkillID);
	bool IsSkillOnCooldown() const;

protected:
	class UStatusComponent* GetStatusComponent(AActor* Owner) const;
	void StartCoolDown(AActor* Owner);

	UPROPERTY()
	AActor* CachedOwner;

	FSkillEntry SkillData;
	int32 SlotKey;
	int32 SkillID;
	FGameplayTag AttackTag;
	float DefaultDamage;

public:
	virtual const FGameplayTag& GetTag() { return AttackTag; }
	const FEnhanceDamage& GetEnhanceConfig() { return SkillData.BaseConfig.EnhanceConfig; }
	const FDash& GetDashConfig() { return SkillData.BaseConfig.DashConfig; }
	const FShockwave& GetShockConfig() { return SkillData.BaseConfig.ShockConfig; }
	const FProjectile& GetProjectileConfig() { return SkillData.BaseConfig.ProjectileConfig; }
	FHitContext GetSkillHitContext();
};
