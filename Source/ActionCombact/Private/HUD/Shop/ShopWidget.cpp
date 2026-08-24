// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Shop/ShopWidget.h"
#include "HUD/Shop/ShopItemSlot.h"
#include "World/ShopActor.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/ShopTypes.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Game/ActionGameInstance.h"

void UShopWidget::BindShop(AShopActor* Shop)
{
	BoundShop = Shop;
	if (!BoundShop.IsValid()) return;

	BoundShop->OnItemPurchased.AddUObject(this, &UShopWidget::HandleItemPurchased);
	CreateEmptySlots();
	RefreshShop();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UShopWidget::HandleCloseRequest);
	}
}

void UShopWidget::RefreshShop()
{
	if (!BoundShop.IsValid()) return;
	
	const TArray<FShopSlotData> ShopItems = BoundShop->GetShopItems();
	for (int idx = 0; idx < FMath::Min(ShopItems.Num(), ShopSlots.Num()); ++idx)
	{
		ShopSlots[idx]->SetSlotData(ShopItems[idx]);
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

		ShopItemSlot->OnPurchaseRequested.AddUObject(this, &UShopWidget::HandlePurchaseRequested);
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

void UShopWidget::HandlePurchaseRequested(const FName& ID)
{
	APawn* Buyer = GetOwningPlayerPawn();
	BoundShop->TryPurchase(Buyer, ID, 1);
}

void UShopWidget::HandleItemPurchased(const int32 Index)
{
	const TArray<FShopSlotData> ShopItems = BoundShop->GetShopItems();
	ShopSlots[Index]->SetSlotData(ShopItems[Index]);
}

void UShopWidget::HandleCloseRequest()
{
	OnShopClosed.Broadcast();
}
