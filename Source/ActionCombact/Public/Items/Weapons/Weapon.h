// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/CombatTypes.h"
#include "Types/TraceTypes.h"
#include "Types/SkillTypes.h"
#include "Types/HitContext.h"
#include "Types/HitContext.h"
#include "Weapon.generated.h"

class UHitEffectDataAsset;
class UNiagaraComponent;
class USkillBase;
class UCombatDataAsset;
class UStatusComponent;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	void Equip(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void DisableSphereCollision();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	virtual FHitContext GetHitContext() { return FHitContext(); }

	virtual void DoTrace() {}
	virtual void ResetTraceIndex() {}
	virtual void ClearPrevLocation() {}
	void ApplySocketPolicy();

	void UseSkillQ();
	void UseSkillE();
	void UseSkillR();

	void PlayNiagaraEffect();
	void DestroyNiagaraEffect();

protected:
	virtual void BeginPlay() override;
	void InitializeSkills();
	FHitContext BuildWeaponHitContext(const TSet<AActor*>& AlreadyHit);
	TArray<AActor*> BuildActorsToIgnore();

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	bool bUseCharacterSocket = false;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	TArray<USkillBase*> Skills;

	UPROPERTY(EditAnywhere, Category = "Combat|Data Asset")
	UCombatDataAsset* WeaponData;

	const FCombatTraceData* CurrentTraceData = nullptr;

private:
	void SetHitEffectData();

	UPROPERTY(EditAnywhere, Category = State)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Combat|Data Asset")
	UHitEffectDataAsset* HitEffectData;

	UPROPERTY(VisibleAnywhere, Category = "Visual Effects")
	UNiagaraComponent* BuffComp;

public:
	virtual FVector GetTraceStart() const { return FVector(); }
	virtual FVector GetTraceEnd() const { return FVector(); }

	FORCEINLINE USceneComponent* GetWeaponMesh() { return ItemMesh; }
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }
	virtual FName GetTraceStartName() { return FName(); }
	USkillBase* GetCurrentSkill(ESkillSlot Slot);

	void SetbUseCharacterSocket(bool Value) { bUseCharacterSocket = Value; }
	virtual void SetTraceIndex(int32 Index) {}
	virtual void SetCombatTraceData(const FCombatTraceData* NewData, const FGameplayTag& Tag) {}
};
