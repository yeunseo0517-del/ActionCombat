#pragma once

#include "CoreMinimal.h"
#include "Types/GameplayTags.h"
#include "HitContext.generated.h"

USTRUCT()
struct FHitContext
{
	GENERATED_BODY()

	AActor* Instigator = nullptr;
	AActor* DamageCauser = nullptr;

	FGameplayTag AttackTag;

	TSet<AActor*> AlreadyHitActors;
};