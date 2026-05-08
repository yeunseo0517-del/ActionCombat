#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Unarmed UMETA(DisplayName = "Unarmed"),
	EWT_Sword UMETA(DisplayName = "Sword")
};

UENUM(BlueprintType)
enum class EWeaponStance : uint8
{
	EWS_Unarmed UMETA(DisplayName = "Unarmed"),
	EWS_OneHand UMETA(DisplayName = "OneHand"),
	EWS_TwoHand UMETA(DisplayName = "TwoHand"),
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	EAT_None UMETA(DisplayName = "None"),
	EAT_BasicAttack UMETA(DisplayName = "BasicAttack"),
	EAT_DashAttack UMETA(DisplayName = "DashAttack"),
	EAT_WaveAttack UMETA(DisplayName = "WaveAttack")
};

USTRUCT(BlueprintType)
struct FCombatState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | State")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | State")
	EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | State")
	EWeaponStance WeaponStance;

	bool operator ==(const FCombatState& Other) const
	{
		return WeaponType == Other.WeaponType &&
			AttackType == Other.AttackType &&
			WeaponStance == Other.WeaponStance;
	}
};

FORCEINLINE uint32 GetTypeHash(const FCombatState& Key)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Key.WeaponType));
	Hash = HashCombine(Hash, GetTypeHash(Key.AttackType));
	Hash = HashCombine(Hash, GetTypeHash(Key.WeaponStance));
	return Hash;
}

USTRUCT(BlueprintType)
struct FAnimMontageArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> Montages;
};