// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/SkillTypes.h"
#include "StatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatusChanged, EStatusType);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, int32, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillActivated, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusComponent();

	void AddStatus(EStatusType Status);
	void RemoveStatus(EStatusType Status);
	bool HasStatus(EStatusType Status);
	void AddEnhancedDamage(float Value);
	void RemoveEnhancedDamage(float Value);
	float GetEnhancedDamage();
	void AddDefense(float Value);
	void RemoveDefense(float Value);
	float GetDefense();
	bool IsSkillOnCooldown(int32 SkillID);
	void ActivateSkill(int32 Slot, int32 SkillID, float Cooldown);
	void SkillDurationEnd(int32 InSlotKey);


	void PrintCurrentStatuses();

	FOnStatusChanged OnStatusStart;
	FOnStatusChanged OnStatusEnd;
	FOnCooldownChanged OnCooldownStart;
	FOnSkillActivated OnSkillActivated;
	FOnSkillActivated OnSkillDeactivated;

	UPROPERTY()
	TMap<EStatusType, int32> StatusCount;

	UPROPERTY()
	TMap<int32, float> CooldownEndTime;

private:
	void StartCooldown(int32 Slot, int32 SkillID, float Cooldown);

	TArray<float> EnhancedDamageSources;
	TArray<float> DefenseSources;
	FString GetStatusTypeString(EStatusType Type);

	TMap<ESkillSlot, FTimerHandle> DurationHandle;
};
