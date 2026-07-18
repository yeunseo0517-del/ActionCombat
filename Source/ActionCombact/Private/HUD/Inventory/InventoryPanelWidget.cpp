// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryPanelWidget.h"
#include "HUD/Inventory/InventoryItemSlot.h"
#include "HUD/Inventory/ItemContextMenuWidget.h"
#include "Components/InventoryComponent.h"
#include "Game/ActionGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Items/ItemBase/ItemBase.h"

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
	ItemSlot->OnItemRightClicked.AddUObject(this, &UInventoryPanelWidget::HandleItemRightClicked);

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
	BoundInventory->RemoveItemByInstanceID(PendingInstanceID);
	if (ItemContextMenu) ItemContextMenu->RemoveFromParent();
}

void UInventoryPanelWidget::HandleDropRequested()
{
	if (BoundInventory.IsValid() && PendingInstanceID.IsValid()) BoundInventory->RemoveItemByInstanceID(PendingInstanceID);
	if (ItemContextMenu) ItemContextMenu->RemoveFromParent();
}
