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

const bool UItemBase::IsFullItemStack() const
{
	return Quantity == ItemData.ItemNumericData.MaxStackSize;
}

const bool UItemBase::IsStackable() const
{
	return ItemData.ItemNumericData.bIsStackable;
}

int32 UItemBase::AddToStack(int32 Amount)
{
	int32 MaxStack = ItemData.ItemNumericData.MaxStackSize;
	int32 TryToAdd = Amount + Quantity;
	if (TryToAdd > MaxStack)
	{
		Quantity = MaxStack;
		return TryToAdd - MaxStack;
	}
	Quantity = TryToAdd;
	return 0;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		int32 MaxSize = ItemData.ItemNumericData.bIsStackable ? ItemData.ItemNumericData.MaxStackSize : 1;
		Quantity = FMath::Clamp(NewQuantity, 0, MaxSize);
	}
}

void UItemBase::Use(ASlashCharacter* Character)
{
}
