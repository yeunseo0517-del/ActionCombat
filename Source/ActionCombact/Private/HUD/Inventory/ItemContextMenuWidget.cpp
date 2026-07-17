// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/ItemContextMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UItemContextMenuWidget::ShowMenuWidget(const FText& InActionText)
{
	if (ActionText) ActionText->SetText(InActionText);
}

void UItemContextMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ActionButton) ActionButton->OnClicked.AddDynamic(this, &UItemContextMenuWidget::HandleActionClicked);
	if (DropButton) DropButton->OnClicked.AddDynamic(this, &UItemContextMenuWidget::HandleDropClicked);
}

void UItemContextMenuWidget::HandleActionClicked()
{
	OnActionClicked.Broadcast();
}

void UItemContextMenuWidget::HandleDropClicked()
{
	OnDropClicked.Broadcast();
}
