#pragma once

#include "CoreMinimal.h"
#include "ProfileData.generated.h"

USTRUCT(BlueprintType)
struct FProfileData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Gold = 0;

    UPROPERTY()
    TArray<int32> ClearedStageIDs;
};