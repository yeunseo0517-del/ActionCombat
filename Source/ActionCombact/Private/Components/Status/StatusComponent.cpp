// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Status/StatusComponent.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusComponent::StartCooldown(ESkillSlot Slot, float Cooldown)
{
	float Now = GetWorld()->GetTimeSeconds();
	float EndTime = Now + Cooldown;
	CooldownEndTime.Add(Slot, EndTime);
	OnCooldownStart.Broadcast(Slot, EndTime, Cooldown);
}

void UStatusComponent::SkillDurationEnd(ESkillSlot Slot)
{
	OnSkillDeactivated.Broadcast(Slot);
}

bool UStatusComponent::IsCooldownActivate(ESkillSlot Slot)
{
	if (float* EndTime = CooldownEndTime.Find(Slot))
	{
		return *EndTime > GetWorld()->GetTimeSeconds();
	}
	return false;
}


void UStatusComponent::ActivateSkill(ESkillSlot Slot, float Cooldown)
{
	OnSkillActivated.Broadcast(Slot);
	StartCooldown(Slot, Cooldown);
}

void UStatusComponent::AddStatus(EStatusType Status)
{
	int32& Count = StatusCount.FindOrAdd(Status);
	Count++;
	if (Count == 1)
	{
		OnStatusStart.Broadcast(Status);
	}
}

void UStatusComponent::RemoveStatus(EStatusType Status)
{
	if (!StatusCount.Contains(Status)) return;

	int32& Count = StatusCount[Status];
	Count = FMath::Max(0, Count - 1);
	if (Count == 0)
	{
		OnStatusEnd.Broadcast(Status);
	}
}

bool UStatusComponent::HasStatus(EStatusType Status)
{
	const int32* Count = StatusCount.Find(Status);
	return Count && (*Count > 0);
}

void UStatusComponent::AddEnhancedDamage(float Value)
{
	EnhancedDamageSources.Add(Value);
}

void UStatusComponent::RemoveEnhancedDamage(float Value)
{
	EnhancedDamageSources.RemoveSingle(Value);
}

float UStatusComponent::GetEnhancedDamage()
{
	float TotalDamage = 0.f;
	for (float Value : EnhancedDamageSources)
		TotalDamage += Value;
	return TotalDamage;
}

FString UStatusComponent::GetStatusTypeString(EStatusType Type)
{
	const UEnum* EnumPtr = StaticEnum<EStatusType>();
	return EnumPtr ? EnumPtr->GetNameStringByValue((int64)Type) : TEXT("Invalid");
}

void UStatusComponent::PrintCurrentStatuses()
{
	UE_LOG(LogTemp, Warning, TEXT("=== Current Active Statuses for %s ==="), *GetOwner()->GetName());

	// CurrentStatuses가 TSet<EStatusType> 혹은 TArray<EStatusType>라고 가정
	if (StatusCount.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No active status."));
		return;
	}

	for (const TPair<EStatusType, int32>& Pair : StatusCount)
	{
		const UEnum* EnumPtr = StaticEnum<EStatusType>();
		FString StatusName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)Pair.Key) : TEXT("Unknown");

		// 카운트가 0보다 큰 것들은 초록색(Display), 0인 것들은 회색(Log)으로 출력
		if (Pair.Value > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT(" > [ACTIVE] %s : %d stacks"), *StatusName, Pair.Value);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT(" > [inactive] %s : %d stacks"), *StatusName, Pair.Value);
		}
	}
}