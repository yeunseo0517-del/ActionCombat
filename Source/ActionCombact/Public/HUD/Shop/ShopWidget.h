// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class AShopActor;
class UShopItemSlot;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE(FOnShopClosed);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindShop(AShopActor* Shop);

	FOnShopClosed OnShopClosed;
private:
	void RefreshShop();
	void CreateEmptySlots();
	void ClearEmptySlots();

	void SetGoldInfo();

	void HandleSlotSelected(const int32 idx);

	UFUNCTION()
	void HandleCloseRequest();

	UPROPERTY()
	TWeakObjectPtr<AShopActor> BoundShop;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UShopItemSlot> ShopSlotClass;

	UPROPERTY()
	TArray<TObjectPtr<UShopItemSlot>> ShopSlots;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* SlotGrid;

	UPROPERTY(meta=(BindWidget))
	class UButton* CloseButton;
};
