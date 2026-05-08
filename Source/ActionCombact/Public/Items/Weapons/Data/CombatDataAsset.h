// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/TraceTypes.h"
#include "Types/CombatTypes.h"
#include "Types/SkillTypes.h"
#include "Types/GameplayTags.h"
#include "CombatDataAsset.generated.h"

class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UCombatDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Combat | Trace")
	TMap<FGameplayTag, FCombatTraceData> AttackSet;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UNiagaraSystem* EnhanceEffect;

	UPROPERTY(EditAnywhere)
	float DefaultDamage;

	UPROPERTY(EditAnywhere, Category = "Skills")
	TArray<FSkillEntry> SkillEntries;
};
