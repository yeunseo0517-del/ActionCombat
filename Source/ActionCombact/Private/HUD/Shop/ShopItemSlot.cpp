// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Shop/ShopItemSlot.h"
#include "Game/ActionGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"

void UShopItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->OnGoldChanged.AddUObject(this, &UShopItemSlot::HandleGoldChanged);
		HandleGoldChanged(GI->GetCurrentGold());
	}

	if (PurchaseButton) PurchaseButton->OnClicked.AddDynamic(this, &UShopItemSlot::HandlePurchaseClicked);

	SetItemContentVisible(false);
}

void UShopItemSlot::HandleGoldChanged(const int32 Amount)
{
	if (CurrentGold) CurrentGold->SetText(FText::AsNumber(Amount));
}

void UShopItemSlot::HandlePurchaseClicked()
{
	OnPurchaseRequested.Broadcast(SlotData.ItemID);
}

void UShopItemSlot::SetItemContentVisible(bool bVisible)
{
	if (TextBox) TextBox->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if (GoldBox) GoldBox->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UShopItemSlot::UpdateTextInfo()
{
	SetItemContentVisible(true);
	if (NameText) NameText->SetText(SlotData.DisplayName);
	if (ItemImage) ItemImage->SetBrushFromTexture(SlotData.Icon);
	if (PriceText) PriceText->SetText(FText::AsNumber(SlotData.Price));
}

void UShopItemSlot::SetSlotData(const FShopSlotData& Data)
{
	SlotData = Data;

	UpdateTextInfo();
}

void UShopItemSlot::ClearSlot()
{
	SlotData = FShopSlotData{};
	SetItemContentVisible(false);
}
