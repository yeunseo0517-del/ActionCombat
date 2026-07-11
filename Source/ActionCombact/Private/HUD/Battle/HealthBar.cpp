// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::SetHealthPercent(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar || !HealthText) return;

	const float Percent = CurrentHealth / MaxHealth;
	HealthBar->SetPercent(Percent);

	const int32 Current = FMath::CeilToInt(CurrentHealth);
	const int32 Max = FMath::CeilToInt(MaxHealth);

	HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
}