// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ShopTypes.h"
#include "ShopItemSlot.generated.h"

class UTextBlock;
class UHorizontalBox;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPurchaseRequested, const FName&);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UShopItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSlotData(const FShopSlotData& Data);
	void ClearSlot();

	FOnPurchaseRequested OnPurchaseRequested;

protected:
	virtual void NativeConstruct() override;

private:
	void HandleGoldChanged(const int32 Amount);

	UFUNCTION()
	void HandlePurchaseClicked();
	void SetItemContentVisible(bool bVisible);
	void UpdateTextInfo();

	FShopSlotData SlotData;

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

	UPROPERTY(meta = (BindWidget))
	class UButton* PurchaseButton;
};
