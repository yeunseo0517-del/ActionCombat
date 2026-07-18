#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/Combat/CombatTypes.h"
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

	UPROPERTY(EditDefaultsOnly)
	float ArmorRating;

	UPROPERTY(EditDefaultsOnly)
	float DamageValue;

	UPROPERTY(EditDefaultsOnly)
	float RestorationAmount;

	UPROPERTY(EditDefaultsOnly)
	float SellValue;
};

USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly)
	FText InteractionText;

	UPROPERTY(EditDefaultsOnly)
	FText UsageText;
};

USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 MaxStackSize;

	UPROPERTY(EditDefaultsOnly)
	bool bIsStackable;
};

USTRUCT(BlueprintType)
struct FItemAssetData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Mesh;
};

USTRUCT(BlueprintType)
struct FWeaponSetting
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<class UCombatDataAsset> CombatData;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<class UHitEffectDataAsset> HitEffectData;
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	TSubclassOf<class UItemBase> ItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data", meta = (EditCondition = "ItemType != EItemType::EIT_Consumable ", EditConditionHides))
	FWeaponSetting WeaponSetting;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	FItemAssetData ItemAssetData;
};