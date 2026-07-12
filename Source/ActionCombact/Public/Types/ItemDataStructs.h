#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM(BlueprintType)
enum class EItemQuality : uint8
{
	EIQ_Common UMETA(DisplayName = "Common"),
	EIQ_Uncommon UMETA(DisplayName = "Uncommon"),
	EIQ_Rare UMETA(DisplayName = "Rare"),
	EIQ_Epic UMETA(DisplayName = "Epic"),
	EIQ_Legnedary UMETA(DisplayName = "Legnedary")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Armor UMETA(DisplayName = "Armor"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Shield UMETA(DisplayName = "Shield"),
	EIT_Spell UMETA(DisplayName = "Spell"),
	EIT_Consumable UMETA(DisplayName = "Consumable")
};

USTRUCT(BlueprintType)
struct FItemStatistics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ArmorRating;

	UPROPERTY(EditAnywhere)
	float DamageValue;

	UPROPERTY(EditAnywhere)
	float RestorationAmount;

	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	FText UsageText;
};

USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT(BlueprintType)
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData ItemAssetData;
};