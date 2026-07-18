// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanelWidget.generated.h"

class UItemBase;
class UItemContextMenuWidget;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindInventory(class UInventoryComponent* Inventory);
	void HideChildWidgets();
	
private:
	virtual void NativeOnInitialized() override;
	void UpdateTextInfo(const int32 Amount) const;
	void HandleInventoryRefreshed(const TArray<TObjectPtr<UItemBase>>& Items);
	void HandleInventoryAdded(UItemBase* Item);
	void HandleInventorRemoved(UItemBase* Item);
	void HandleInventoryUpdated(UItemBase* Item);
	void HandleItemRightClicked(const FGuid& InstanceID, const FVector2D Position, const FText& UsageText);
	void HandleActionRequested();
	void HandleDropRequested();

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> BoundInventory;

	UPROPERTY()
	class UItemBase* PendingItem;

	FGuid PendingInstanceID;

	UPROPERTY()
	UItemContextMenuWidget* ItemContextMenu;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* InventoryPanel;

	UPROPERTY(meta =(BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UInventoryItemSlot> SlotClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemContextMenuWidget> ContextMenuClass;
};
