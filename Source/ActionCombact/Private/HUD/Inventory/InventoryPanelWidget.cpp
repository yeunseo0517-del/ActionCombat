// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Inventory/InventoryPanelWidget.h"
#include "Components/InventoryComponent.h"
#include "Game/ActionGameInstance.h"
#include "Components/TextBlock.h"

//OnInventoryUpdated

void UInventoryPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->OnGoldChanged.AddUObject(this, &UInventoryPanelWidget::UpdateTextInfo);
}

void UInventoryPanelWidget::UpdateTextInfo(const int32 Amount) const
{
	GoldText->SetText(FText::AsNumber(Amount));
}

void UInventoryPanelWidget::HandleInventoryRefreshed(const TArray<TObjectPtr<UItemBase>>& Items)
{
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
	}

	BoundInventory = NewInventory;
	if (!BoundInventory.IsValid()) return;

	BoundInventory->OnInventoryRefresh.AddUObject(this, &UInventoryPanelWidget::HandleInventoryRefreshed);
	HandleInventoryRefreshed(BoundInventory->GetInventoryContents());
}