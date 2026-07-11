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
	void SetItemData(const FItemData& Data, const int32 InQuantity);

	FORCEINLINE bool IsFullItemStack() const;
	FORCEINLINE const FItemData& GetItemData() const { return ItemData; }
	FORCEINLINE int32 GetQuantity() const { return Quantity; }

	void SetQuantity(const int32 NewQuantity);
	virtual void Use(class ASlashCharacter* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ItemData.ItemID == OtherID;
	}

private:
	//UPROPERTY()
	// UInventoryComponent* OwingInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;
};
