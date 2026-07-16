// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UAttributeComponent;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindAttribute(UAttributeComponent* Attribute);

private:
	UPROPERTY()
	TWeakObjectPtr<UAttributeComponent> BoundAttribute;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	void SetHealthPercent(float CurrentHealth, float MaxHealth);
};
