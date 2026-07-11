// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Interaction/InteractionWidget.h"
#include "Components/TextBlock.h"
#include "Types/InteractionTypes.h"

void UInteractionWidget::UpdateWidget(const FInteractableData& InteractableData)
{
	if (Name) Name->SetText(InteractableData.Name);
	if (ActionText) ActionText->SetText(InteractableData.ActionText);
}