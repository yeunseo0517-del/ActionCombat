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

UENUM(BlueprintType)
enum class ESkillRange : uint8
{
	Far, // 0
	Mid, // 1
};

USTRUCT(BlueprintType)
struct FEnhanceDamage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float SkillDamage = 10.f;

	UPROPERTY(EditAnywhere)
	float Duration = 5.f;
};

USTRUCT(BlueprintType)
struct FDash
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float DashDistance = 300.f;
};

USTRUCT(BlueprintType)
struct FShockwave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARadialShockwaves> ShockwaveClass;

	UPROPERTY(EditAnywhere)
	float SkillDamage = 50.f;

	UPROPERTY(EditAnywhere)
	float Duration = 15.f;

	UPROPERTY(EditAnywhere)
	double MaxRadius = 1300.f;
};

USTRUCT(BlueprintType)
struct FProjectile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	float SkillDamage = 50.f;

	UPROPERTY(EditAnywhere)
	float Speed = 2000.f;

	UPROPERTY(EditAnywhere)
	float Range = 1500.f;
};

USTRUCT(BlueprintType)
struct FSkillConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Cooldown = 8.f;

	UPROPERTY(EditAnywhere)
	FEnhanceDamage EnhanceConfig;

	UPROPERTY(EditAnywhere)
	FDash DashConfig;

	UPROPERTY(EditAnywhere)
	FShockwave ShockConfig;

	UPROPERTY(EditAnywhere)
	FProjectile ProjectileConfig;
};

USTRUCT(BlueprintType)
struct FSkillEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USkillBase> SkillClass;

	UPROPERTY(EditAnywhere)
	FSkillConfig BaseConfig;
};