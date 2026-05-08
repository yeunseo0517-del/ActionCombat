// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/SkillTypes.h"
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
	float GetDistance() const { return MoveDistance; }
	

protected:
	class UStatusComponent* GetStatusComponent(AActor* Owner) const;
	void StartCoolDown(AActor* Owner);

	UPROPERTY()
	AActor* CachedOwner;

	ESkillSlot SkillSlot;
	float Duration;
	float Cooldown;
	float MoveDistance;
};
