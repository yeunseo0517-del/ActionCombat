// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthPercent(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		const float Percent = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(Percent);
	}

	if (HealthText)
	{
		const int32 Current = FMath::CeilToInt(CurrentHealth);
		const int32 Max = FMath::CeilToInt(MaxHealth);

		HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
	}
}

void USlashOverlay::SetStaminaPercent(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar)
	{
		const float Percent = CurrentStamina / MaxStamina;
		StaminaBar->SetPercent(Percent);
	}

	if (StaminaText)
	{
		const int32 Current = FMath::CeilToInt(CurrentStamina);
		const int32 Max = FMath::CeilToInt(MaxStamina);

		StaminaText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	UE_LOG(LogTemp, Warning, TEXT("Update: %d"), Gold);
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}