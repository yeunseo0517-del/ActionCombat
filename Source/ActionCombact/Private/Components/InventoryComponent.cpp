// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/Item/ItemAddResult.h"
#include "Items/ItemBase/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UItemBase* UInventoryComponent::FindItemByInstanceID(const FGuid& InstanceID)
{
	if (!InstanceID.IsValid()) return nullptr;
	const auto* Result = Items.FindByPredicate([&InstanceID](const TObjectPtr<UItemBase>& Item)
		{ return Item && Item->GetInstanceID() == InstanceID; });
	return Result ? Result->Get() : nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* Target) const
{
	if (!Target) return nullptr;
	const auto* Result = Items.FindByPredicate([Target](const TObjectPtr<UItemBase>& Item)
		{ return Item &&
		Item->GetItemID() == Target->GetItemID() &&
		!Item->IsFullItemStack(); });
	return Result ? Result->Get() : nullptr;
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* Target)
{
	if (!Target) return FItemAddResult();

	int32 TotalAmount = Target->GetQuantity();
	int32 RemainAmount = TotalAmount;
	if (TotalAmount <= 0) return FItemAddResult();

	FItemAddResult Result = FItemAddResult();
	if (!Target->IsStackable())
	{
		Result = HandleNonStackableItem(Target);
	}
	else
	{
		UItemBase* Item = FindNextPartialStack(Target);
		// 추가 가능한 기존 아이템이 있는 경우
		if (Item)
		{
			FillExistingStacks(Item, RemainAmount);
		}

		AddItems(Target, RemainAmount);

		if (RemainAmount == TotalAmount) Result = FItemAddResult::AddedNone(FullText);
		else if (RemainAmount > 0) Result = FItemAddResult::AddedPartial(TotalAmount - RemainAmount, PartialSuccessText);
		else Result = FItemAddResult::AddedAll(TotalAmount, SuccessText);
	}

	OnItemAddCompleted.Broadcast(Result, Target->GetItemName(), Target->IsStackable());
	return Result;
}

FItemAddResult UInventoryComponent::HandleNonStackableItem(UItemBase* Target)
{
	int32 RemainAmount = 1;
	AddItems(Target, RemainAmount);

	if(RemainAmount) return FItemAddResult::AddedNone(FullText);
	return FItemAddResult::AddedAll(1, SuccessText);
}

void UInventoryComponent::FillExistingStacks(UItemBase* Target, int32& RemainingAmount)
{
	while (Target && RemainingAmount > 0)
	{
		RemainingAmount = Target->AddToStack(RemainingAmount);
		OnInventoryUpdated.Broadcast(Target);
		Target = FindNextPartialStack(Target);
	}
}

void UInventoryComponent::AddItems(UItemBase* Target, int32& RemainingAmount)
{
	if (RemainingAmount <= 0) return;

	const int32 MaxStack = Target->GetMaxStackSize();
	while (RemainingAmount > 0 && Items.Num() < SlotsCapacity)
	{
		UItemBase* NewItem = Target->CreateItemCopy();
		NewItem->SetInstanceID();
		int32 AddAmount = FMath::Min(RemainingAmount, MaxStack);

		NewItem->SetQuantity(AddAmount);
		RemainingAmount -= AddAmount;

		Items.Add(NewItem);
		OnInventoryAdded.Broadcast(NewItem);
	}
}

void UInventoryComponent::RemoveSingleItem(UItemBase* Item)
{
	Items.RemoveSingle(Item);
	OnInventoryRemoved.Broadcast(Item);
}

int32 UInventoryComponent::RemoveAmountItem(UItemBase* Target, int32 RequestAmount)
{
	if (!Target || RequestAmount <= 0) return 0;
	
	int32 CurrentAmount = Target->GetQuantity();
	if (RequestAmount >= CurrentAmount)
	{
		RemoveSingleItem(Target);
		return CurrentAmount;
	}
	
	int32 RemoveAmount = CurrentAmount - RequestAmount;
	Target->SetQuantity(RemoveAmount);
	OnInventoryUpdated.Broadcast(Target);
	return RequestAmount;
}

void UInventoryComponent::SplitStack(UItemBase* Target, int32 Amount)
{
	if (!Target || Items.Num() >= SlotsCapacity) return;
	if (Amount >= Target->GetQuantity() || Amount <= 0) return;

	RemoveAmountItem(Target, Amount);
	AddItems(Target, Amount);
}

void UInventoryComponent::RemoveItemByInstanceID(const FGuid& ID)
{
	UItemBase* Target = FindItemByInstanceID(ID);

	if (!Target) return;
	if (Target->IsStackable())
	{
		RemoveAmountItem(Target, 1);
	}
	else
	{
		RemoveSingleItem(Target);
	}
}

//void UInventoryComponent::RemoveAmountItemByID(const FGuid& ID, int32 RequestAmount)
//{
//	UItemBase* Target = FindItemByInstanceID(ID);
//	if (!Target) return;
//	RemoveAmountItem(Target, RequestAmount);
//}

void UInventoryComponent::UseItem(const FGuid& InstanceID)
{
	UItemBase* Target = FindItemByInstanceID(InstanceID);
	if (!Target) return;
	Target->UseItem(GetOwner());
}

void UInventoryComponent::CaptureSaveData(FProfileData& Profile)
{
	Profile.InventoryItems.Empty();

	for (UItemBase* Item : Items)
	{
		if (!Item) continue;
		FInventorySaveData SaveData;
		SaveData.ItemID = Item->GetItemID();
		SaveData.Guid = Item->GetInstanceID();
		SaveData.Quantity = Item->GetQuantity();

		Profile.InventoryItems.Add(SaveData);
	}

	Profile.EquippedWeapon.ItemID = EquippedWeaponBase->GetItemID();
	Profile.EquippedWeapon.Guid = EquippedWeaponBase->GetInstanceID();
}

void UInventoryComponent::RestoreSaveData(const FProfileData& Profile)
{
	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please Set Item Data Table in Inventory Component"));
		return;
	}

	RestoreInventoryData(Profile);
	RestoreEquippedWeapon(Profile.EquippedWeapon);
}

void UInventoryComponent::RestoreInventoryData(const FProfileData& Profile)
{
	Items.Empty();

	for (const FInventorySaveData& SaveData : Profile.InventoryItems)
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(SaveData.ItemID, SaveData.ItemID.ToString());
		if (!ItemData || !ItemData->ItemClass) continue;

		UItemBase* Item = NewObject<UItemBase>(this, ItemData->ItemClass);
		if (!Item) continue;

		Item->SetItemData(*ItemData, SaveData.Quantity);
		Item->SetInstanceID(SaveData.Guid);

		Items.Add(Item);
	}

	OnInventoryRefresh.Broadcast(Items);
}

void UInventoryComponent::RestoreEquippedWeapon(const FEquipmentSaveData& EquipSaveData)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *EquipSaveData.ItemID.ToString())
	const FItemData* WeaponData = ItemDataTable->FindRow<FItemData>(EquipSaveData.ItemID, EquipSaveData.ItemID.ToString());
	if (!WeaponData || !WeaponData->ItemClass) return;

	UItemBase* Item = NewObject<UItemBase>(this, WeaponData->ItemClass);
	if (!Item) return;

	Item->SetItemData(*WeaponData, 1);
	Item->SetInstanceID(EquipSaveData.Guid);

	Item->UseItem(GetOwner());
}
