// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryTooltip.h"
#include "HUD/Inventory/InventoryItemSlot.h"
#include "Items/ItemBase/ItemBase.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UInventoryTooltip::InitializeTooltip(UItemBase* ItemBeingHovered)
{
	if (!ItemBeingHovered) return;

	const FItemData& ItemData = ItemBeingHovered->GetItemData();
	SetTextInfo(ItemData, ItemBeingHovered->GetQuantity());
	SetToolTip(this);
}

void UInventoryTooltip::SetTextInfo(const FItemData& ItemData, const int32 Quantity)
{
	if (ItemName) ItemName->SetText(ItemData.ItemTextData.Name);
	if (DamageValue)
	{
		if (ItemData.ItemStatistics.DamageValue != 0)
			DamageValue->SetText(FText::Format(NSLOCTEXT("Item", "AttackStat", "+{0}"), FText::AsNumber(ItemData.ItemStatistics.DamageValue)));
		else
		{
			SetVisibilityCollapsed(DamageValue);
			if (DamageText) SetVisibilityCollapsed(DamageText);
		}
	}
	if (ArmorRating)
	{
		if (ItemData.ItemStatistics.ArmorRating != 0)
			ArmorRating->SetText(FText::Format(NSLOCTEXT("Item", "ArmorStat", "+{0}"), FText::AsNumber(ItemData.ItemStatistics.ArmorRating)));
		else
		{
			SetVisibilityCollapsed(ArmorRating);
			if (ArmorRatingText) SetVisibilityCollapsed(ArmorRatingText);
		}
	}
	if (Restoration)
	{
		if (ItemData.ItemStatistics.RestorationAmount != 0)
			Restoration->SetText(FText::Format(NSLOCTEXT("Item", "RestoreStat", "+{0}"), FText::AsNumber(ItemData.ItemStatistics.RestorationAmount)));
		else
		{
			SetVisibilityCollapsed(Restoration);
			if (RestorationText) SetVisibilityCollapsed(RestorationText);
		}
	}
	if (ItemDescription) ItemDescription->SetText(ItemData.ItemTextData.Description);
	if (SellValue) SellValue->SetText(FText::AsNumber(ItemData.ItemStatistics.SellValue));

	if (ItemData.ItemNumericData.bIsStackable)
	{
		if (StackValue) StackValue->SetText(FText::Format(NSLOCTEXT("Item", "StackVal", "{0}/{1}"), FText::AsNumber(Quantity), FText::AsNumber(ItemData.ItemNumericData.MaxStackSize)));
	}
	else
	{
		if (StackValue) SetVisibilityCollapsed(StackValue);
	}
}

void UInventoryTooltip::SetVisibilityCollapsed(UTextBlock* TextBlock)
{
	TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}