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
	void TryProcessTarget(AActor* Target, FVector ImpactPoint);

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

	bool HasProjectile();
	bool HasShockwave();
	void SpawnRadialShockwave();
	void SpawnProjectile();

	void HandleHitResult(AActor* HitActor, FVector ImpactPoint);
	void ExecuteGetHit(AActor* Hit, FVector ImpactPoint);
	void SpawnHitSparkParticles(FVector ImpactPoint);
	bool IsHostile(AActor* Actor);
	float CalculateDamage(float DefaultDamage);
	bool ProcessDamageApplication(AActor* Target);

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

public:
	void SetbTracing(bool Value) { bTracing = Value; }
	void SetCombatTraceData();
	FGameplayTag GetCurrentCombatTag() { return CurrentCombatTag; }
	void SetCurrentSkill(USkillBase* NewSkill) { CurrentSkill = NewSkill; UE_LOG(LogTemp, Warning, TEXT("CurrentSkill Ptr: %p"), CurrentSkill);
	}
	void SetHitEffectData(UHitEffectDataAsset* NewEffect) { CurHitEffectData = NewEffect; }
	void SetHitContext(FHitContext& NewContext) { CurHitContext = NewContext; }
};