// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "EnhanceSkill.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ACTIONCOMBACT_API UEnhanceSkill : public USkillBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateSkill(AActor* Owner) override;
	virtual void Init(const FSkillEntry& Config) override;

protected:

private:
	UFUNCTION()
	void DeactivateSkill();

	float EnhanceDamage;

	FTimerHandle EnhanceTimer;
};
