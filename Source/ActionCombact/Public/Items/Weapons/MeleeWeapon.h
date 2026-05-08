// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMeleeWeapon();

	virtual void BeginPlay() override;

	virtual void DoTrace() override;

protected:
	virtual void ClearIgnoreArray() override;
private:
	void ExecuteSweepTrace(const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& Hits);
	void UpdatePrevLocations(const FVector& Start, const FVector& End, const FVector& Center);
	void DoSphereTrace(const FVector Start, const FVector End, const float Radius, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& Hits);
	void DoBoxTraceMulti(const FVector Start, const FVector End, const TArray<AActor*>& ActorsToIgnore, TArray<FHitResult>& Hits);
	FRotator GetRotation();

	void SetTraceIgnoreActors(TArray<AActor*>& ActorsToIgnore);
	FVector GetTrace(FName SocketName) const;

	bool bHasPrevLocation = false;
	FVector PrevStartLocation;
	FVector PrevEndLocation;
	FVector PrevCenter;
	int32 CurrentTraceIndex = 0;

	const FCombatTraceData* CurrentTraceData = nullptr;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;


public:
	virtual FVector GetTraceStart() const override;
	virtual FVector GetTraceEnd() const override;
	virtual FName GetTraceStartName() override;

	virtual void ClearPrevLocation() override { bHasPrevLocation = false; }
	virtual void SetTraceIndex(int32 Index) override { CurrentTraceIndex = Index; }
	virtual void ResetTraceIndex() override { CurrentTraceIndex = 0; }

	virtual void SetCombatTraceData(const FCombatTraceData* NewData, const FGameplayTag& Tag) override;
};
