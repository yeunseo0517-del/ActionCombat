// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UResultWidget::HandleCloseButtonClicked);
}

void UResultWidget::HandleCloseButtonClicked()
{
	OnCloseRequested.Broadcast();
}


void UResultWidget::SetBattleResult(const FBattleResult& InBattleResult)
{
	if (Victory)
	{
		const FText VictoryText = InBattleResult.bVictory ? FText::FromString(TEXT("Stage Clear")) : FText::FromString(TEXT("Stage Failed"));
		Victory->SetText(VictoryText);
	}

	if (KilledEnemy) KilledEnemy->SetText(FText::AsNumber(InBattleResult.KilledEnemyCount));
}