// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/ItemDataStructs.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UItemBase : public UObject
{
	GENERATED_BODY()
	
public:
	UItemBase();

	UItemBase* CreateItemCopy() const;

	FORCEINLINE float GetItemStackWeight() const;
	FORCEINLINE float GetItemSingleWeight() const;
	FORCEINLINE bool IsFullItemStack() const;

	void SetQuantity(const int32 NewQuantity);
	virtual void Use(class ASlashCharacter* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ItemID == OtherID;
	}

private:
	//UPROPERTY()
	// UInventoryComponent* OwingInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ItemID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemQuality Quality;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData TextData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData NumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData AssetData;
};
