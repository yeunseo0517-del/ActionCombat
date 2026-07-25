// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Shop/ShopWidget.h"
#include "HUD/Shop/ShopItemSlot.h"
#include "World/ShopActor.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/ShopTypes.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

constexpr int32 ColumnCount = 1;
constexpr int32 TotalSlotCount = 6;

void UShopWidget::BindShop(AShopActor* Shop)
{
	BoundShop = Shop;
	if (!BoundShop.IsValid()) return;
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UShopWidget::HandleCloseRequest);
	}
	CreateEmptySlots();
	RefreshShop();
}

void UShopWidget::RefreshShop()
{
	if (!BoundShop.IsValid()) return;
	
	for (int idx = 0; idx < FMath::Min(BoundShop->GetShopItems().Num(), ShopSlots.Num()); ++idx)
	{
		const FShopSlotData& Data = BoundShop->GetShopItems()[idx];
		ShopSlots[idx]->SetSlotData(Data, idx);
	}
	ClearEmptySlots();
}

void UShopWidget::CreateEmptySlots()
{
	for (int i = 0; i < TotalSlotCount; ++i)
	{
		UShopItemSlot* ShopItemSlot = CreateWidget<UShopItemSlot>(this, ShopSlotClass);
		const int32 Row = i / ColumnCount;
		const int32 Column = i % ColumnCount;
		SlotGrid->AddChildToUniformGrid(ShopItemSlot, Row, Column);
		ShopSlots.Add(ShopItemSlot);

		ShopItemSlot->OnShopItemSelected.AddUObject(this, &UShopWidget::HandleSlotSelected);
	}
}

void UShopWidget::ClearEmptySlots()
{
	const int32 ItemCount = BoundShop->GetShopItems().Num();
	for (int i = ItemCount; i < TotalSlotCount; ++i)
	{
		ShopSlots[i]->ClearSlot();
	}
}

void UShopWidget::SetGoldInfo()
{
	/*const int32 Item
	if(TotalPrice) TotalPrice->SetText(ShopSlots[0])*/
}

void UShopWidget::HandleSlotSelected(const int32 idx)
{
	
}

void UShopWidget::HandleCloseRequest()
{
	OnShopClosed.Broadcast();
}
