// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ATreasure : public AItem
{
	GENERATED_BODY()

protected:
	ATreasure();

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	int32 GoldAmount = 0;

public:
	void SetGoldAmount(int32 Amount);
};
