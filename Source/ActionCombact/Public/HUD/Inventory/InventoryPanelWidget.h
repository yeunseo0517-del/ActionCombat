// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanelWidget.generated.h"

class UItemBase;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindInventory(class UInventoryComponent* Inventory);
	
private:
	virtual void NativeOnInitialized() override;
	void UpdateTextInfo(const int32 Amount) const;
	void HandleInventoryRefreshed(const TArray<TObjectPtr<UItemBase>>& Items);

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> BoundInventory;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UInventoryItemSlot> SlotClass;

	UPROPERTY(meta =(BindWidget))
	class UTextBlock* GoldText;
};
