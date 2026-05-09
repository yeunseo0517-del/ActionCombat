#pragma once

#include "CoreMinimal.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class EStatusType : uint8
{
	EST_Invincible UMETA(DisplayName = "Invincible"),
	EST_EnhancedDamage UMETA(DisplayName = "Enhanced Damage"),
	EST_SuperArmor UMETA(DisplayName = "Super Armor")
};

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	Q, // 0
	E, // 1
	R // 2
};

USTRUCT(BlueprintType)
struct FSkillConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Cooldown = 8.f;

	UPROPERTY(EditAnywhere)
	float EnhanceDamage;

	UPROPERTY(EditAnywhere)
	float SkillDamage;

	UPROPERTY(EditAnywhere)
	float Duration;

	UPROPERTY(EditAnywhere)
	float DashDistance;

	UPROPERTY(EditAnywhere)
	double MaxRadius;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Niagara;
};

USTRUCT(BlueprintType)
struct FSkillEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USkillBase> SkillClass;

	UPROPERTY(EditAnywhere)
	FSkillConfig BaseConfig;

	UPROPERTY(VisibleAnywhere)
	ESkillSlot SkillSlot;
};