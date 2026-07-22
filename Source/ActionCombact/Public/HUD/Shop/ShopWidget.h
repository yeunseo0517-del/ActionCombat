// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class AShopActor;
class UShopItemSlot;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindShop(AShopActor* Shop);

private:
	void RefreshShop();

	UPROPERTY()
	TWeakObjectPtr<AShopActor> BoundShop;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UShopItemSlot> ShopSlotClass;
};
