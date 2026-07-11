// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Interaction/AcquiredNotificationWidget.h"
#include "Components/TextBlock.h"
#include "Types/InteractionTypes.h"

void UAcquiredNotificationWidget::UpdateNotification(const FInteractableData& Data)
{
	if (Name) Name->SetText(Data.Name);
	if (ActionText) ActionText->SetText(Data.ActionText);
	if (Quantity) Quantity->SetText(FText::AsNumber(Data.Quantity));
}