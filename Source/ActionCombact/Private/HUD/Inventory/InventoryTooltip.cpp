// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryTooltip.h"
#include "HUD/Inventory/InventoryItemSlot.h"
#include "Items/ItemBase.h"
#include "Components/TextBlock.h"

void UInventoryTooltip::InitializeTooltip(UItemBase* ItemBeingHovered)
{
	if (!ItemBeingHovered) return;
	UE_LOG(LogTemp, Warning, TEXT("In Tooltip"))
	const FItemData& ItemData = ItemBeingHovered->GetItemData();

	if (ItemName) ItemName->SetText(ItemData.ItemTextData.Name);
	if (DamageValue)
	{
		if (ItemData.ItemStatistics.DamageValue != 0)
			DamageValue->SetText(FText::AsNumber(ItemData.ItemStatistics.DamageValue));
		else
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ArmorRating)
	{
		if (ItemData.ItemStatistics.ArmorRating != 0)
			ArmorRating->SetText(FText::AsNumber(ItemData.ItemStatistics.ArmorRating));
		else
			ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (UsageText) UsageText->SetText(ItemData.ItemTextData.UsageText);
	if (ItemDescription) ItemDescription->SetText(ItemData.ItemTextData.Description);
	if (SellValue) SellValue->SetText(FText::AsNumber(ItemData.ItemStatistics.SellValue));

	if (ItemData.ItemNumericData.bIsStackable)
	{
		if (MaxStackSize) MaxStackSize->SetText(FText::AsNumber(ItemData.ItemNumericData.MaxStackSize));
		if (StackSizeValue) StackSizeValue->SetText(FText::AsNumber(ItemBeingHovered->GetQuantity()));
	}
	else
	{
		if (MaxStackSize) MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
		if (StackSizeValue) StackSizeValue->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetToolTip(this);
}