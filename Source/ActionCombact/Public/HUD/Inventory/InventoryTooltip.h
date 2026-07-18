// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltip.generated.h"

class UInventoryItemSlot;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeTooltip(class UItemBase* Instance);

	void SetTextInfo(const struct FItemData& ItemData, const int32 Quantity);

private:
	void SetVisibilityCollapsed(UTextBlock* TextBlock);

	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* SlotBeingHovered;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArmorRating;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArmorRatingText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Restoration;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RestorationText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StackValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SellValue;
};
