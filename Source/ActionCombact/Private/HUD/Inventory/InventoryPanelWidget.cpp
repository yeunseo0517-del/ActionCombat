// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryPanelWidget.h"
#include "HUD/Inventory/InventoryItemSlot.h"
#include "HUD/Inventory/ItemContextMenuWidget.h"
#include "Components/InventoryComponent.h"
#include "Game/ActionGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Items/ItemBase/ItemBase.h"

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

void UInventoryPanelWidget::HideChildWidgets()
{
	if (ItemContextMenu) ItemContextMenu->RemoveFromParent();
}

void UInventoryPanelWidget::UpdateTextInfo(const int32 Amount) const
{
	GoldText->SetText(FText::AsNumber(Amount));
}

void UInventoryPanelWidget::HandleInventoryRefreshed(const TArray<TObjectPtr<UItemBase>>& Items)
{
	if (!SlotClass || !SlotGrid) return;
	SlotGrid->ClearChildren();
	ItemSlots.Empty();

	CreateEmptySlots();

	const int32 Count = FMath::Min(Items.Num(), ItemSlots.Num());
	for (int i = 0; i < Count; ++i)
	{
		ItemSlots[i]->SetItemInstance(Items[i]);
	}
}

void UInventoryPanelWidget::CreateEmptySlots()
{
	if (!BoundInventory.IsValid()) return;
	for (int i = 0; i < BoundInventory->GetCapacity(); ++i)
	{
		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, SlotClass);
		const int32 Row = i / ColumnCount;
		const int32 Column = i % ColumnCount;
		SlotGrid->AddChildToUniformGrid(ItemSlot, Row, Column);
		ItemSlots.Add(ItemSlot);

		ItemSlot->OnItemRightClicked.AddUObject(this, &UInventoryPanelWidget::HandleItemRightClicked);
	}
}

void UInventoryPanelWidget::HandleInventoryAdded(UItemBase* Item)
{
	int32 Index = BoundInventory->GetInventoryContents().Num() - 1;
	if (!ItemSlots.IsValidIndex(Index)) return;
	ItemSlots[Index]->SetItemInstance(Item);
}

void UInventoryPanelWidget::HandleInventorRemoved(UItemBase* Item)
{
	const int32 ItemCount = BoundInventory->GetInventoryContents().Num();
	for (int32 i = 0; i < ItemCount; ++i)
	{
		if (ItemSlots[i]->GetItemInstance() == Item)
		{
			ItemSlots[i]->ClearSlot();
			return;
		}
	}
}

void UInventoryPanelWidget::HandleInventoryUpdated(UItemBase* Item)
{
	const int32 ItemCount = BoundInventory->GetInventoryContents().Num();
	for (int32 i = 0; i < ItemCount; ++i)
	{
		if (ItemSlots[i]->GetItemInstance() == Item)
		{
			ItemSlots[i]->UpdateInfo();
			return;
		}
	}
}

void UInventoryPanelWidget::HandleItemRightClicked(const FGuid& InstanceID, const FVector2D Position, const FText& UsageText)
{
	if (ItemContextMenu)
	{
		ItemContextMenu->RemoveFromParent();
	}
	if (!ContextMenuClass) return;
	ItemContextMenu = CreateWidget<UItemContextMenuWidget>(this, ContextMenuClass);
	if (!ItemContextMenu) return;

	PendingInstanceID = InstanceID;

	ItemContextMenu->OnActionClicked.AddUObject(this, &UInventoryPanelWidget::HandleActionRequested);
	ItemContextMenu->OnDropClicked.AddUObject(this, &UInventoryPanelWidget::HandleDropRequested);

	ItemContextMenu->AddToViewport(200);
	ItemContextMenu->SetPositionInViewport(Position, false);
	ItemContextMenu->ShowMenuWidget(UsageText);
}

void UInventoryPanelWidget::HandleActionRequested()
{
	if (!BoundInventory.IsValid() || !PendingInstanceID.IsValid()) return;
	BoundInventory->UseItem(PendingInstanceID);
	BoundInventory->RemoveItemByInstanceID(PendingInstanceID, 1);
	if (ItemContextMenu) ItemContextMenu->RemoveFromParent();
}

void UInventoryPanelWidget::HandleDropRequested()
{
	if (BoundInventory.IsValid() && PendingInstanceID.IsValid()) BoundInventory->RemoveItemByInstanceID(PendingInstanceID, 10);
	if (ItemContextMenu) ItemContextMenu->RemoveFromParent();
}
