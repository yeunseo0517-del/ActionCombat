#pragma once

#include "CoreMinimal.h"
#include "Types/GameplayTags.h"
#include "TraceTypes.generated.h"

UENUM(BlueprintType)
enum class ETraceType : uint8
{
	ETT_BoxSingle UMETA(DisplayName = "Box Single"),
	ETT_BoxSweep UMETA(DisplayName = "Box Sweep"),
	ETT_Sphere UMETA(DisplayName = "Sphere")
};

USTRUCT(BlueprintType)
struct FComboTrace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Combo")
	FName StartSocket = FName("TraceStart");

	UPROPERTY(EditAnywhere, Category = "Combo")
	FName EndSocket = FName("TraceEnd");
};

USTRUCT(BlueprintType)
struct FCombatTraceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ETraceType Shape;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Shape == ETraceType::ETT_BoxSingle || Shape == ETraceType::ETT_BoxSweep", EditConditionHides))
	FVector HalfSize;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Shpae == ETraceType::ETT_Sphere", EditConditionHides))
	float Radius;

	UPROPERTY(EditAnywhere)
	FGameplayTag AttackTag;

	UPROPERTY(EditAnywhere, Category = "Combo")
	TArray<FComboTrace> Steps;
};