// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloodFieldActor.generated.h"

UCLASS()
class BLOODFIELD_API ABloodFieldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABloodFieldActor();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> FieldBounds;

	UPROPERTY(EditAnywhere, Category = "Blood")
	FVector3f FieldScale = { 3000.f, 3000.f, 500.f };
};
