// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Shop/ShopWidget.h"
#include "HUD/Shop/ShopItemSlot.h"
#include "World/ShopActor.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/ShopTypes.h"

void UShopWidget::BindShop(AShopActor* Shop)
{
	//if (BoundShop.Get() == Shop)
	//{
	//	// Refresh
	//	return;
	//}
	//if (BoundShop.IsValid())
	//{
	//	// remove Delegate
	//}

	BoundShop = Shop;
	if (!BoundShop.IsValid()) return;

	// Bind Delegate
	// Refresh
}

void UShopWidget::RefreshShop()
{
	if (!BoundShop.IsValid()) return;
	for (const FShopSlotData& Data : BoundShop->GetShopItems())
	{

	}
}
