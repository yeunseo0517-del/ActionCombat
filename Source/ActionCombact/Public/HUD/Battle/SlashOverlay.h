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
	void BindAttribute(class UAttributeComponent* Attribute);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void SetStaminaPercent(float CurrentStamina, float MaxStamina) const;
	void SetGold(const int32 Gold) const;

	UPROPERTY(meta = (BindWidget))
	class UHealthBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StaminaText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;
};
