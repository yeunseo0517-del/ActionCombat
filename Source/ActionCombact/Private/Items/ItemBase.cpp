// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"

UItemBase::UItemBase()
{
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(GetOuter(), GetClass());
	ItemCopy->ItemData = this->ItemData;
	return ItemCopy;
}

void UItemBase::SetItemData(const FItemData& Data, const int32 InQuantity)
{
	ItemData = Data;
	Quantity = InQuantity;
}

bool UItemBase::IsFullItemStack() const
{
	return Quantity == ItemData.ItemNumericData.MaxStackSize;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		int32 MaxSize = ItemData.ItemNumericData.bIsStackable ? ItemData.ItemNumericData.MaxStackSize : 1;
		Quantity = FMath::Clamp(NewQuantity, 0, MaxSize);

		/*if (OwningInventory)
		{
		}*/
	}
}

void UItemBase::Use(ASlashCharacter* Character)
{
}
