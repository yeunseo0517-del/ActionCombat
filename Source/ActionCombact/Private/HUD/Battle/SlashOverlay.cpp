// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/SlashOverlay.h"
#include "Game/ActionGameInstance.h"
#include "HUD/Battle/HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Attribute/AttributeComponent.h"

void USlashOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;

	GI->OnGoldChanged.RemoveAll(this);

	GI->OnGoldChanged.AddUObject(this, &USlashOverlay::SetGold);
	SetGold(GI->GetCurrentGold());
}

void USlashOverlay::BindAttribute(UAttributeComponent* Attribute)
{
	if (HealthBar) HealthBar->BindAttribute(Attribute);
}

void USlashOverlay::SetStaminaPercent(float CurrentStamina, float MaxStamina) const
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

void USlashOverlay::SetGold(const int32 Gold) const
{
	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void USlashOverlay::NativeDestruct()
{
	if (UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance()))
	{
		GI->OnGoldChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}