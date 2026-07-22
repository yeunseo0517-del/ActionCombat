// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SaveableInterface.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, const float, const float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UAttributeComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Heal(float Value);

	virtual void CaptureSaveData(FProfileData& Profile) override;
	virtual void RestoreSaveData(const FProfileData& SaveData) override;

	float GetAttackPower() const { return BaseAttackPower; }
	float GetDefense() const { return BaseDefense; }

	FOnHealthChanged OnHealthChanged;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = Attributes)
	float Health;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float BaseAttackPower;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float BaseDefense;

public:
	void RecieveDamage(float DamageAmount);
	float GetMaxHealth();
	float GetCurrentHealth();
	bool IsAlive();
};
