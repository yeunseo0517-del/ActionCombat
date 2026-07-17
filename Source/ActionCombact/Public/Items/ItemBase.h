// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/Item/ItemDataStructs.h"
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

	int32 AddToStack(int32 Amount);
	void SetQuantity(const int32 NewQuantity);
	virtual void Use(class ASlashCharacter* Character);

	const bool IsFullItemStack() const;
	const bool IsStackable() const;
	const FItemData& GetItemData() const { return ItemData; }
	const FName& GetItemID() const { return ItemData.ItemID; }
	const FText& GetItemName() const { return ItemData.ItemTextData.Name; }
	const FText& GetActionText() const { return ItemData.ItemTextData.InteractionText; }
	const int32 GetQuantity() const { return Quantity; }
	const EItemQuality GetQuality() const { return ItemData.ItemQuality; }
	const int32 GetMaxStackSize() const { return ItemData.ItemNumericData.MaxStackSize; }
	class UTexture2D* GetItemIcon() const { return ItemData.ItemAssetData.Icon; }

	void SetInstanceID() { if (!InstanceID.IsValid()) InstanceID = FGuid::NewGuid(); }
	const FGuid& GetInstanceID() { return InstanceID; }

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ItemData.ItemID == OtherID;
	}

private:
	UPROPERTY()
	FGuid InstanceID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;
};
