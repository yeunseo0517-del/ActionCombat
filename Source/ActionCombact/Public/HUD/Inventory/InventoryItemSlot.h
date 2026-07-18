// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

class UItemBase;
class UInventoryTooltip;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemRightClicked, const FGuid&, const FVector2D, const FText& ActionText);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemInstance(UItemBase* ItemIn) { ItemInstance = ItemIn; }
	UItemBase* GetItemInstance() const { return ItemInstance; }
	void UpdateInfo();

	FOnItemRightClicked OnItemRightClicked;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void SetBorderColor();
	void SetIconImage();
	void SetQuantityText();

	UPROPERTY(VisibleAnywhere, Category = "Item Slot")
	UItemBase* ItemInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Item Slot")
	TSubclassOf<UInventoryTooltip> TooltipClass;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ItemBorder;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemQuantity;
};
