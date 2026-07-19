// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SaveableInterface.h"
#include "InventoryComponent.generated.h"

class UItemBase;
class UWeaponItem;
struct FItemAddResult;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryRefresh, const TArray<TObjectPtr<UItemBase>>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, UItemBase*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAddCompleted, const FItemAddResult&, const FText&, const bool IsStackable);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UInventoryComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//int32 GetSlotsCapacity() const { return SlotsCapacity; }
	TArray<TObjectPtr<UItemBase>> GetInventoryContents() const { return Items; }

	FItemAddResult HandleAddItem(UItemBase* Item);
	void SplitStack(UItemBase* Item, int32 Amount);
	void RemoveItemByInstanceID(const FGuid& ID);
	//void RemoveAmountItemByID(const FGuid& ID, int32 RequestAmount);
	void UseItem(const FGuid& InstanceID);

	virtual void CaptureSaveData(FProfileData& Profile) override;
	virtual void RestoreSaveData(const FProfileData& SaveData) override;
	void SetWeaponBase(UItemBase* NewData) { EquippedWeaponBase = NewData; }

	FOnInventoryRefresh OnInventoryRefresh;

	FOnInventoryUpdated OnInventoryUpdated;
	FOnInventoryUpdated OnInventoryAdded;
	FOnInventoryUpdated OnInventoryRemoved;

	FOnItemAddCompleted OnItemAddCompleted;

protected:
	virtual void BeginPlay() override;

private:
	UItemBase* FindItemByInstanceID(const FGuid& InstanceID);
	UItemBase* FindNextPartialStack(UItemBase* Item) const;
	FItemAddResult HandleNonStackableItem(UItemBase* Item);
	void FillExistingStacks(UItemBase* Target, int32& RemainingAmount);
	void AddItems(UItemBase* Target, int32& RemainingAmount);
	void RemoveSingleItem(UItemBase* Item);
	int32 RemoveAmountItem(UItemBase* Item, int32 RemoveAmount);

	void RestoreInventoryData(const FProfileData& Profile);
	void RestoreEquippedWeapon(const FEquipmentSaveData& Profile);

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	UDataTable* ItemDataTable;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<UItemBase>> Items;

	UPROPERTY(VisibleInstanceOnly, Category = Weapon)
	UItemBase* EquippedWeaponBase;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 SlotsCapacity = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory | Text")
	FText FullText = FText::FromString(TEXT("ÀÎº¥Åä¸®°¡ °¡µæ Ã¡½À´Ï´Ù."));

	UPROPERTY(EditDefaultsOnly, Category = "Inventory | Text")
	FText SuccessText = FText::FromString(TEXT("À»(¸¦) È¹µæÇß½À´Ï´Ù."));

	UPROPERTY(EditDefaultsOnly, Category = "Inventory | Text")
	FText PartialSuccessText = FText::FromString(TEXT("È¹µæ - ³²Àº °ø°£ÀÌ ¾ø½À´Ï´Ù."));
};
