// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/CombatTypes.h"
#include "Types/TraceTypes.h"
#include "Types/GameplayTags.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABaseCharacter;
class UCombatDataAsset;
class USkillBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBACT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckAndTriggerNextCombo();
	void ResetCombo();

	void BasicAttack();
	void ExecuteSkillAttack(EAttackType AttackType);
	void AttackEnd(bool bInterrupted);
	bool IsAttackMontage(UAnimMontage* Montage);
	void OnWeaponEquipped(AWeapon* NewWeapon);
	void ExecuteSkill(FGameplayTag SkillTag);
	void ExecuteAction(FGameplayTag ActionTag);

protected:
	virtual void BeginPlay() override;

private:
	void ExecuteAttackSequence(EAttackType AttackType);
	bool CanAttack();
	void PlayAttackMontage();
	bool IsAttacking();
	void HandleCombo();
	void UpdateComboState();
	FName GetComboSectionName(UAnimMontage* Montage);
	FAnimMontageArray* GetMontageArray();
	FCombatState GetCurrentCombatState();
	FCombatTraceData* GetCombatTraceData(EAttackType AttackType) const;
	void SetAttackType(EAttackType AttackType);
	UAnimMontage* GetCurrentAttackMontage();
	void StartDash();
	void EndDash();

	UPROPERTY(EditAnywhere, Category = "Combat")
	UCombatDataAsset* OverrideCombatData;

	UPROPERTY(VisibleInstanceOnly)
	AWeapon* EquippedWeapon;

	UPROPERTY()
	USkillBase* CurrentSkill;

	bool bIsDashing;
	FVector DashTargetLoc;

	UPROPERTY()
	ABaseCharacter* Character;

	UPROPERTY(EditAnywhere, Category = Montage)
	TMap<FCombatState, FAnimMontageArray> AnimMap;

	UPROPERTY(VisibleInstanceOnly)
	int32 ComboCount = 0;

	UPROPERTY()
	bool bUseCombo = false;
	UPROPERTY()
	bool bNextCombo;
	UPROPERTY()
	bool bComboTriggerd;
	
	UPROPERTY()
	bool bTracing;

	UPROPERTY(VisibleAnywhere)
	EAttackType CurrentAttackType = EAttackType::EAT_None;

public:
	void SetbTracing(bool Value) { bTracing = Value; }
	void SetCombatTraceData();
	void SetCurrentSkill(USkillBase* NewSkill) { CurrentSkill = NewSkill; }
};
