// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurroundSlotComponent.generated.h"


USTRUCT()
struct FSurroundSlot
{
	GENERATED_BODY()

	float Angle;
	FVector Direction;

	UPROPERTY()
	TWeakObjectPtr<AActor> Occupant = nullptr;
};

struct FEnemySlotScore
{
	TWeakObjectPtr<AActor> Enemy;
	int32 SlotIndex = INDEX_NONE;
	float Cost = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API USurroundSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USurroundSlotComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RequestSlot(AActor* Requester);
	void ReleaseSlot(AActor* Requester);
	//bool CanKeepSlot(AActor* Requester);
	bool GetAssignedSlotLocation(AActor* Requester, FVector& OutLocation) const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void AssignSlots();
	int32 FindAssignedSlotIndex(AActor* Requester) const;
	FVector GetSlotWorldLocation(int32 Index) const;

	UPROPERTY(EditAnywhere, Category = "Slot")
	int32 SlotCount;

	UPROPERTY(EditAnywhere, Category = "Slot")
	float Radius = 250.f;

	UPROPERTY(EditAnywhere, Category = "Slot")
	float SlotReleaseMargin = 50.f;

	UPROPERTY()
	TArray<FSurroundSlot> SurroundSlots;

	bool bAssignSlots = false;
	TArray<int32> FreeSlotIndices;
	TArray<FEnemySlotScore> SlotScores;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> PendingEnemies;
};
