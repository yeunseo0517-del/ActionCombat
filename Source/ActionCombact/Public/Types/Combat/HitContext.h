#pragma once

#include "CoreMinimal.h"
#include "Types/GameplayTags.h"
#include "HitContext.generated.h"


USTRUCT()
struct FHitInfo
{
	GENERATED_BODY()

	FVector ImpactPoint;
	FVector HitDir;
};