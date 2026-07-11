// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Equipped UMETA(DisplayName = "Equipped")
};

class USphereComponent;

UCLASS()
class ACTIONCOMBACT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(EditAnywhere, Category = "Sine Parameters")
	float Amplitude;

	UPROPERTY(EditAnywhere, Category = "Sine Parameters")
	float TimeConstant;

	UFUNCTION(BlueprintPure)
	float TransformedSin();
};
