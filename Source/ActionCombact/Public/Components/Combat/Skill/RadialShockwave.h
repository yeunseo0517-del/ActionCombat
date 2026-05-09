// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "RadialShockwave.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API URadialShockwave : public USkillBase
{
	GENERATED_BODY()
	
	virtual void ActivateSkill(AActor* Owner) override;
	virtual void Init(const FSkillEntry& Config) override;
};
