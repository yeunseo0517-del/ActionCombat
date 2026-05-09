// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/CombatTypes.h"
#include "Types/TraceTypes.h"
#include "Types/GameplayTags.h"
#include "Types/HitContext.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ABaseCharacter;
class UCombatDataAsset;
class USkillBase;
class UHitEffectDataAsset;

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
	void AttackEnd(bool bInterrupted);
	bool IsAttackMontage(UAnimMontage* Montage);
	void OnWeaponEquipped(AWeapon* NewWeapon);
	void ExecuteAttack(const FGameplayTag& Tag);
	void ExecuteAction(const FGameplayTag& Tag);
	void OnAttackWindow();

	void ProcessHitResults(TArray<FHitResult>& HitResults);

protected:
	virtual void BeginPlay() override;

private:
	void ExecuteAttackSequence(FGameplayTag Tag);
	bool CanAttack();
	void PlayAttackMontage();
	bool IsAttacking();
	void HandleCombo();
	void UpdateComboState();
	FName GetComboSectionName(UAnimMontage* Montage);
	FAnimMontageArray* GetMontageArray();
	FCombatState GetCurrentCombatState();
	FCombatTraceData* GetCombatTraceData(const FGameplayTag& Tag) const;
	void SetCurrentCombatTag(const FGameplayTag& Tag);
	UAnimMontage* GetCurrentAttackMontage();
	void StartDash();
	void EndDash();

	void HandleHitResult(const FHitResult& HitResult);
	void ExecuteGetHit(const FHitResult& HitResult);
	void SpawnHitSparkParticles(const FHitResult& HitResult);
	bool IsHostile(AActor* Actor);
	float CalculateDamage(float DefaultDamage);

	void ExpandImpactRadius();
	double CalculateRadiusFromOwner();
	void SpawnShockwave(double Radius);
	void DoOverlap();

	UPROPERTY(EditAnywhere, Category = "Combat")
	UCombatDataAsset* OverrideCombatData;

	UPROPERTY()
	UHitEffectDataAsset* CurHitEffectData;

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

	bool bUseCombo = false;
	bool bNextCombo;
	bool bComboTriggerd;
	bool bTracing;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag CurrentCombatTag = FGameplayTag();

	UPROPERTY()
	FHitContext CurHitContext;

	double CurRadius;
	double MaxRadius;
	float Speed;
	bool bExpandRadius = false;

	UPROPERTY()
	class UNiagaraComponent* EffectComp;

	UPROPERTY()
	class UNiagaraSystem* Shockwave;

public:
	void SetbTracing(bool Value) { bTracing = Value; }
	void SetCombatTraceData();
	void SetCurrentSkill(USkillBase* NewSkill) { CurrentSkill = NewSkill; }
	void SetHitEffectData(UHitEffectDataAsset* NewEffect) { CurHitEffectData = NewEffect; }
	void SetHitContext(FHitContext& NewContext) { CurHitContext = NewContext; }
};
