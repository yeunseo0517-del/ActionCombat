// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/HealthBar.h"
#include "Components/Attribute/AttributeComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::BindAttribute(UAttributeComponent* NewAttribute)
{
	if (BoundAttribute.Get() == NewAttribute)
	{
		SetHealthPercent(NewAttribute->GetCurrentHealth(), NewAttribute->GetMaxHealth());
	}
	if (BoundAttribute.IsValid())
	{
		BoundAttribute->OnHealthChanged.RemoveAll(this);
	}

	BoundAttribute = NewAttribute;
	if (!BoundAttribute.Get()) return;

	BoundAttribute->OnHealthChanged.AddUObject(this, &UHealthBar::SetHealthPercent);
	SetHealthPercent(NewAttribute->GetCurrentHealth(), NewAttribute->GetMaxHealth());
}

void UHealthBar::SetHealthPercent(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar) return;

	const float Percent = CurrentHealth / MaxHealth;
	HealthBar->SetPercent(Percent);

	if (!HealthText) return;
	const int32 Current = FMath::CeilToInt(CurrentHealth);
	const int32 Max = FMath::CeilToInt(MaxHealth);

	HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
}