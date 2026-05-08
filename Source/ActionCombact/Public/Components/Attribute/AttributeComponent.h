// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RecieveDamage(float DamageAmount);
	float GetHealthPercent();
	bool IsAlive();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float Health;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float MaxHealth;
};
