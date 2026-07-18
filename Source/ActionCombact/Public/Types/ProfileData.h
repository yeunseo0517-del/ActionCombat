#pragma once

#include "CoreMinimal.h"
#include "ProfileData.generated.h"

USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	int32 Quantity = 0;

	UPROPERTY()
	FGuid Guid;
};

USTRUCT()
struct FEquipmentSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	FGuid Guid;
};

USTRUCT(BlueprintType)
struct FProfileData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Gold = 0;

    UPROPERTY()
    TArray<int32> ClearedStageIDs;

	UPROPERTY()
	TArray<FInventorySaveData> InventoryItems;

	UPROPERTY()
	FEquipmentSaveData EquippedWeapon;
};