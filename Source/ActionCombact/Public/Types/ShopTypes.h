#pragma once

#include "CoreMinimal.h"
#include "ShopTypes.generated.h"

USTRUCT()
struct FShopEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ItemID;

	UPROPERTY(EditAnywhere)
	int32 Stock = -1;

	UPROPERTY(EditAnywhere)
	float PriceMultiplier = 1.f;
};

USTRUCT()
struct FShopSlotData
{
	GENERATED_BODY()

	UPROPERTY()
	FName ItemID;

	UPROPERTY()
	FText DisplayName;

	UPROPERTY()
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY()
	int32 Price = 0;

	UPROPERTY()
	int32 Stock = -1;

	UPROPERTY()
	bool bCanPurchase = false;
};