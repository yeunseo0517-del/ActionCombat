// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"

UItemBase::UItemBase()
{
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());
	ItemCopy->ItemID = this->ItemID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->Quality = this->Quality;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->AssetData = this->AssetData;
	return ItemCopy;
}

float UItemBase::GetItemStackWeight() const
{
	return Quantity * NumericData.Weight;
}

float UItemBase::GetItemSingleWeight() const
{
	return NumericData.Weight;
}

bool UItemBase::IsFullItemStack() const
{
	return Quantity == NumericData.MaxStackSize;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		int32 MaxSize = NumericData.bIsStackable ? NumericData.MaxStackSize : 1;
		Quantity = FMath::Clamp(NewQuantity, 0, MaxSize);

		/*if (OwningInventory)
		{
		}*/
	}
}

void UItemBase::Use(ASlashCharacter* Character)
{
}
