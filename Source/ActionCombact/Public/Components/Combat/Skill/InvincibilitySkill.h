// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "InvincibilitySkill.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInvincibilitySkill : public USkillBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateSkill(AActor* Owner) override;
};
