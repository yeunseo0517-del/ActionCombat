#pragma once

#include "CoreMinimal.h"
#include "BattleResult.generated.h"

USTRUCT(BlueprintType)
struct FBattleResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bVictory = false;

    UPROPERTY(BlueprintReadOnly)
    int32 KilledEnemyCount = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 ClearedStageID = 0;
};