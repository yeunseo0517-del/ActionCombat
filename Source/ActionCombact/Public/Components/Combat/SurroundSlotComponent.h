// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurroundSlotComponent.generated.h"


USTRUCT()
struct FSurroundSlot
{
	GENERATED_BODY()

	FVector Direction;

	UPROPERTY()
	TWeakObjectPtr<AActor> Occupant = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API USurroundSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USurroundSlotComponent();

	int32 RequestSlot(AActor* Requester);
	FVector GetSlotWorldLocation(int32 Index);
	void ReleaseSlot(int32 Index);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	UPROPERTY(EditAnywhere, Category = "Slot")
	int32 SlotCount;

	UPROPERTY(EditAnywhere, Category = "Slot")
	float Radius = 50.f;

	UPROPERTY()
	TArray<FSurroundSlot> SurroundSlots;
};
