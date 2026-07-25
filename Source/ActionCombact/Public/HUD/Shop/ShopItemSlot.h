// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ShopTypes.h"
#include "ShopItemSlot.generated.h"

class UTextBlock;
class UHorizontalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const int32);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UShopItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSlotData(const FShopSlotData& Data, const int32 Index);
	void ClearSlot();

	FOnShopItemSelected OnShopItemSelected;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void HandleGoldChanged(const int32 Amount);
	void SetItemContentVisible(bool bVisible);
	void UpdateTextInfo();

	int32 SlotIndex = INDEX_NONE;
	FShopSlotData SlotData;

	FName ItemID;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGold;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* GoldBox;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* TextBox;
};
