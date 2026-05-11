// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/SkillTypes.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusChanged, EStatusType, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, int32, SlotKey, float, EndTime, float, Cooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillActivated, int32, SlotKey);

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
	bool IsSkillOnCooldown(int32 SkillID);
	void ActivateSkill(int32 Slot, int32 SkillID, float Cooldown);
	void SkillDurationEnd(int32 InSlotKey);


	void PrintCurrentStatuses();

	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnStatusStart;

	UPROPERTY(BlueprintAssignable)
	FOnStatusChanged OnStatusEnd;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownStart;

	UPROPERTY(BlueprintAssignable)
	FOnSkillActivated OnSkillActivated;

	UPROPERTY(BlueprintAssignable)
	FOnSkillActivated OnSkillDeactivated;

	UPROPERTY()
	TMap<EStatusType, int32> StatusCount;

	UPROPERTY()
	TMap<int32, float> CooldownEndTime;

private:
	void StartCooldown(int32 Slot, int32 SkillID, float Cooldown);

	TArray<float> EnhancedDamageSources;
	FString GetStatusTypeString(EStatusType Type);

	TMap<ESkillSlot, FTimerHandle> DurationHandle;
};
