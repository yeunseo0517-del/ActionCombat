// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryItemSlot.h"
#include "HUD/Inventory/InventoryTooltip.h"
#include "Items/ItemBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (TooltipClass)
	{
		UInventoryTooltip* Tooltip = CreateWidget<UInventoryTooltip>(this, TooltipClass);
		if (Tooltip)
		{
			Tooltip->InitializeTooltip(ItemInstance);
			if(ItemBorder) ItemBorder->SetToolTip(Tooltip);
		}
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (!ItemInstance) return;

	SetBorderColor();
	SetIconImage();
	SetQuantityText();
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (ItemInstance)
		{
			OnItemRightClicked.Broadcast(
				ItemInstance,
				InMouseEvent.GetScreenSpacePosition()
			);
		}

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryItemSlot::SetBorderColor()
{
	if (!ItemBorder) return;
	switch (ItemInstance->GetQuality())
	{
	case EItemQuality::EIQ_Common:
		ItemBorder->SetBrushColor(FLinearColor::Gray);
		break;
	case EItemQuality::EIQ_Uncommon:
		ItemBorder->SetBrushColor(FLinearColor::Yellow);
		break;
	case EItemQuality::EIQ_Epic:
		ItemBorder->SetBrushColor(FLinearColor::Green);
		break;
	case EItemQuality::EIQ_Rare:
		ItemBorder->SetBrushColor(FLinearColor::Blue);
		break;
	case EItemQuality::EIQ_Legnedary:
		ItemBorder->SetBrushColor(FLinearColor::Red);
		break;
	default:
		break;
	}
}

void UInventoryItemSlot::SetIconImage()
{
	if (!ItemIcon) return;
	ItemIcon->SetBrushFromTexture(ItemInstance->GetItemIcon());
}

void UInventoryItemSlot::SetQuantityText()
{
	if (!ItemQuantity) return;
	if (ItemInstance->IsStackable())
	{
		ItemQuantity->SetVisibility(ESlateVisibility::Visible);
		ItemQuantity->SetText(FText::AsNumber(ItemInstance->GetQuantity()));
	}
	else
	{
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryItemSlot::UpdateInfo()
{
	SetQuantityText();
}