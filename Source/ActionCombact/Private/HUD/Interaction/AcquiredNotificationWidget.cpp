// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Interaction/AcquiredNotificationWidget.h"
#include "Components/TextBlock.h"
#include "Types/InteractionTypes.h"

void UAcquiredNotificationWidget::UpdateNotification(const FInteractableData& Data)
{
	if (Name) Name->SetText(Data.Name);
	bool bShouldShowQuantity = Data.bShowQuantity;
	if (bShouldShowQuantity)
	{
		if (Quantity)
		{
			Quantity->SetText(FText::AsNumber(Data.Quantity));
			Quantity->SetVisibility(ESlateVisibility::Visible);
		}
		if (QuantityPiece) QuantityPiece->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (Quantity) Quantity->SetVisibility(ESlateVisibility::Collapsed);
		if (QuantityPiece) QuantityPiece->SetVisibility(ESlateVisibility::Collapsed);
	}
}