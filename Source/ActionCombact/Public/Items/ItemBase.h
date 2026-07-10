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
		return ItemID == OtherID;
	}

private:
	//UPROPERTY()
	// UInventoryComponent* OwingInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item Data", meta = (UIMin = 1, UIMax = 100))
	int32 Quantity;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality Quality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData NumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData AssetData;
};
