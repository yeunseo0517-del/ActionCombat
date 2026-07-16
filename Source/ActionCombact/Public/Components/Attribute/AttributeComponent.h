// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, const float, const float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnHealthChanged OnHealthChanged;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = Attributes)
	float Health;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float MaxHealth;

public:
	void RecieveDamage(float DamageAmount);
	float GetMaxHealth();
	float GetCurrentHealth();
	bool IsAlive();
};
