// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/Projectiles/BaseProjectile.h"
#include "LinearWaveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ALinearWaveProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	ALinearWaveProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Init(const FProjectile& Config) override;

private:
	virtual void BeginPlay() override;
	void ProcessHitResults(const TArray<FHitResult>& Hits);
	void SpawnLinearProjectile();

	FVector LastLocation;
	FVector StartLocation;
	float MaxDist;

	UPROPERTY()
	UNiagaraComponent* LinearWaveComp;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* LinearWave;
};
