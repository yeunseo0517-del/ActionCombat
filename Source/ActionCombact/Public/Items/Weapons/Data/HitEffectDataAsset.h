// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HitEffectDataAsset.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UHitEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects, Meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* HitSparkSystem;

	UPROPERTY()
	UNiagaraComponent* HitSpark;
};
