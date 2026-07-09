// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent(float CurrentHealth, float MaxHealth);
	void SetStaminaPercent(float CurrentStamina, float MaxStamina);
	void SetGold(int32 Gold);
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StaminaText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
};
