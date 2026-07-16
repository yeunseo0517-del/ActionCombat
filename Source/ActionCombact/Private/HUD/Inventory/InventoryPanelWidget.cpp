// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryPanelWidget.h"
#include "HUD/Inventory/InventoryItemSlot.h"
#include "Components/InventoryComponent.h"
#include "Game/ActionGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

//OnInventoryUpdated

void UInventoryPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->OnGoldChanged.AddUObject(this, &UInventoryPanelWidget::UpdateTextInfo);
	UpdateTextInfo(GI->GetCurrentGold());
}

void UInventoryPanelWidget::BindInventory(UInventoryComponent* NewInventory)
{
	if (BoundInventory.Get() == NewInventory)
	{
		HandleInventoryRefreshed(BoundInventory->GetInventoryContents());
		return;
	}
	if (BoundInventory.IsValid())
	{
		BoundInventory->OnItemAddCompleted.RemoveAll(this);
		BoundInventory->OnInventoryRefresh.RemoveAll(this);
		BoundInventory->OnInventoryAdded.RemoveAll(this);
		BoundInventory->OnInventoryRemoved.RemoveAll(this);
		BoundInventory->OnInventoryUpdated.RemoveAll(this);
	}

	BoundInventory = NewInventory;
	if (!BoundInventory.IsValid()) return;

	BoundInventory->OnInventoryRefresh.AddUObject(this, &UInventoryPanelWidget::HandleInventoryRefreshed);
	BoundInventory->OnInventoryAdded.AddUObject(this, &UInventoryPanelWidget::HandleInventoryAdded);
	BoundInventory->OnInventoryRemoved.AddUObject(this, &UInventoryPanelWidget::HandleInventorRemoved);
	BoundInventory->OnInventoryUpdated.AddUObject(this, &UInventoryPanelWidget::HandleInventoryUpdated);
	HandleInventoryRefreshed(BoundInventory->GetInventoryContents());
}

void UInventoryPanelWidget::UpdateTextInfo(const int32 Amount) const
{
	GoldText->SetText(FText::AsNumber(Amount));
}

void UInventoryPanelWidget::HandleInventoryRefreshed(const TArray<TObjectPtr<UItemBase>>& Items)
{
	if (!SlotClass) return;
 	InventoryPanel->ClearChildren();

	for (const TObjectPtr<UItemBase>& Item : Items)
	{
		HandleInventoryAdded(Item);
	}
}

void UInventoryPanelWidget::HandleInventoryAdded(UItemBase* Item)
{
	UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, SlotClass);
	ItemSlot->SetItemInstance(Item);

	InventoryPanel->AddChildToWrapBox(ItemSlot);
}

void UInventoryPanelWidget::HandleInventorRemoved(UItemBase* Item)
{
	for (int32 i = 0; i < InventoryPanel->GetChildrenCount(); ++i)
	{
		UInventoryItemSlot* Widget = Cast< UInventoryItemSlot>(InventoryPanel->GetChildAt(i));
		if (Widget && Widget->GetItemInstance() == Item)
		{
			InventoryPanel->RemoveChildAt(i);
			return;
		}
	}
}

void UInventoryPanelWidget::HandleInventoryUpdated(UItemBase* Item)
{
	for (int32 i = 0; i < InventoryPanel->GetChildrenCount(); ++i)
	{
		UInventoryItemSlot* Widget = Cast< UInventoryItemSlot>(InventoryPanel->GetChildAt(i));
		if (Widget && Widget->GetItemInstance() == Item)
		{
			Widget->UpdateInfo();
			return;
		}
	}
}
