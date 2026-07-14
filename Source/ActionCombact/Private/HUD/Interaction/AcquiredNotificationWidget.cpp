// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Interaction/AcquiredNotificationWidget.h"
#include "Components/TextBlock.h"
#include "Types/InteractionTypes.h"
#include "Types/Item/ItemAddResult.h"
#include "Components/InventoryComponent.h"

void UAcquiredNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetTextsVisible(false);
}

void UAcquiredNotificationWidget::NativeDestruct()
{
	if (BoundInventory.IsValid())
	{
		BoundInventory->OnItemAddCompleted.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UAcquiredNotificationWidget::BindInventory(UInventoryComponent* NewInventory)
{
	if (BoundInventory == NewInventory) return;
	if (BoundInventory.IsValid())
	{
		BoundInventory->OnItemAddCompleted.RemoveAll(this);
	}

	BoundInventory = NewInventory;
	if (!BoundInventory.IsValid()) return;

	BoundInventory->OnItemAddCompleted.AddUObject(this, &UAcquiredNotificationWidget::ShowWidget);
}

void UAcquiredNotificationWidget::ShowWidget(const FItemAddResult& Result, const FText& Name, const bool IsStackable)
{
	SetTextsVisible(true);
	UpdateNotification(Name, IsStackable, Result.ActualAmountAdded, Result.ResultMessage);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(NotificationDestroyTimer, this, &UAcquiredNotificationWidget::HideWidget, NotiDestroyTime, false);
	}
}

void UAcquiredNotificationWidget::HideWidget()
{
	SetTextsVisible(false);
}


void UAcquiredNotificationWidget::UpdateNotification(const FText& InName, const bool IsStackable, const int32 InQuantity, const FText& InAction)
{
	
	if (NameText) NameText->SetText(InName);
	if (ActionText) ActionText->SetText(InAction);
	bool bShouldShowQuantity = IsStackable;
	if (bShouldShowQuantity)
	{
		if (Quantity)
		{
			Quantity->SetText(FText::AsNumber(InQuantity));
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

void UAcquiredNotificationWidget::SetTextsVisible(bool Value)
{
	if (NameText) NameText->SetVisibility(Value? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (ActionText) ActionText->SetVisibility(Value ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (Quantity) Quantity->SetVisibility(Value ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (QuantityPiece) QuantityPiece->SetVisibility(Value ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
