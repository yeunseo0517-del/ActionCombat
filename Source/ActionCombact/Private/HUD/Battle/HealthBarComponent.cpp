// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/HealthBarComponent.h"
#include "HUD/Battle/HealthBar.h"

void UHealthBarComponent::SetHealthPercent(float CurrentHealth, float MaxHealth)
{
	if (!HealthBarWidget)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->SetHealthPercent(CurrentHealth, MaxHealth);
	}
}
