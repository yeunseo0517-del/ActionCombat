// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/Combat/HitContext.h"
#include "RadialShockwaves.generated.h"

UCLASS()
class ACTIONCOMBACT_API ARadialShockwaves : public AActor
{
	GENERATED_BODY()
	
public:
	ARadialShockwaves();
	virtual void Tick(float DeltaTime) override;
	void InitShockwave(double MaxRadius, float Duration);
	void ExpandImpactRadius();

protected:
	virtual void BeginPlay() override;

private:
	double CalculateRadiusFromOwner();
	void SpawnShockwave(double Radius);
	void OnOverlap();
	void ProcessHitResults(const TArray<FOverlapResult>& OverlapResults);

	UPROPERTY()
	class UNiagaraComponent* EffectComp;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Effect;

	UPROPERTY(VisibleInstanceOnly)
	double CurRadius;

	double MaxRadius;
	float Duration;
	float Speed;
	bool bExpandRadius = false;
};
