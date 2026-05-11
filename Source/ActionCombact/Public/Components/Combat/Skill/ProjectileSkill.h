// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "ProjectileSkill.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UProjectileSkill : public USkillBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateSkill(AActor* Owner) override;
	virtual void Init(const FSkillEntry& Config, int32 SlotKey, int32 SkillID);
};
