// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/WeaponHolderInterface.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Interfaces/TeamInterface.h"
#include "Types/CombatTypes.h"
#include "Types/SkillTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UCombatComponent;
class UAttributeComponent;
class UHitEffectDataAsset;
class UStatusComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USkillBase;

UCLASS()
class ACTIONCOMBACT_API ABaseCharacter : public ACharacter, public ICombatInterface, public IHitInterface, public IWeaponHolderInterface, public ITeamInterface, public IStatusReceiverInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/*
	Combat Interface override function
	*/
	virtual void EnableTrace() override;
	virtual void DisableTrace() override;
	virtual void CheckAndTriggerNextCombo() override;
	virtual void ResetCombo() override;

	virtual USkeletalMeshComponent* GetCombatMesh() const override;
	virtual class UCombatComponent* GetCombatComponent() const override;

	/*
		Weapon Holder Interface override function
	*/
	virtual FVector GetTraceStart() const override;
	virtual FVector GetTraceEnd() const override;

	virtual USceneComponent* GetWeaponMesh() override;
	virtual FName GetTraceStartName() override;

	virtual void HandleEquipState() override {};

	virtual void SwitchSccket(int32 Index) override;
	virtual void ResetSocket() override;

	/*
		Hit Interface override function
	*/
	virtual void GetHit(const FVector& ImpactPoint, UHitEffectDataAsset* HitEffectData, AActor* Hitter) override;
	//

	/*
		Team Interface override function
	*/
	virtual ETeamType GetTeamType() const override { return TeamType; }
	//

	/*
		Status Receiver Interface
	*/
	virtual UStatusComponent* GetStatusComponent() override { return StatusComponent; }
	//

	void Attack(const FGameplayTag& Tag);
	void PlayAttackMontage(UAnimMontage* Montage, FName SectionName);

	virtual void StartAttack();
	virtual void AttackEnd() {}
	virtual bool CanStartAttack() { return false; }

protected:
	virtual void BeginPlay() override;

	void StopMontage();
	void SwitchToWeapon(AWeapon* NewWeapon);
	virtual void Die(const FName& Section);
	virtual void OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted) {}
	virtual void EnterHitReact();

	void SetCurrentState(FGameplayTag Tag);
	bool IsUnoccupied();
	bool IsAttacking();
	bool IsHitReacting();
	bool IsDead();


	/*
	Animation  Montage function
	*/
	void PlayMontage(UAnimMontage* Montage, FName SectionName);
	bool IsHitReactMontage(UAnimMontage* Montage);


	UPROPERTY(VisibleInstanceOnly, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleInstanceOnly, Category = Weapon)
	AWeapon* DefaultWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category = State)
	EWeaponStance WeaponStance;

	UPROPERTY(EditAnywhere, Category = State)
	ETeamType TeamType;

	UPROPERTY()
	UStatusComponent* StatusComponent;

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer ActionTags;

	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer EffectTags;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag CurrentStateTag = FGameplayTag();

private:
	void SpawnDefaultWeapon();
	AWeapon* GetActiveWeapon() const;
	void StartCollisionTimer();
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayHitSound(UHitEffectDataAsset* HitEffectData, const FVector& ImpactPoint);
	void PlayHitReactMontage(const FName& Section);
	void PlayDeathMontage(const FName& Section);
	void OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);
	bool IsAlive();
	void BindDeathDelegate();
	double DirectionalHitReact(const FVector& ImpactPoint);
	FName GetHitSection(double Theta);
	bool IsSuperArmor();
	bool IsInvincible();
	void AddTag(FGameplayTagContainer& Container, const FGameplayTag& Tag);
	void RemoveTag(FGameplayTagContainer& Container, const FGameplayTag& Tag);
	bool HasTag(const FGameplayTagContainer& Container, const FGameplayTag& Tag);

	UFUNCTION()
	void HandleStatusStart(EStatusType Status);

	UFUNCTION()
	void HandleStatusEnd(EStatusType Status);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void DisableMeshCollision();

	/*
		Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName DefaultWeaponSocket = FName("RightHandSocket");

	FTimerHandle CollisionTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UNiagaraSystem* ShieldEffect;

	UPROPERTY()
	UNiagaraComponent* ShieldComp;

public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE EWeaponStance GetWeaponStance() const { return WeaponStance; }
	void AddActionTag(const FGameplayTag& Tag);
	void RemoveActionTag(const FGameplayTag& Tag);
	bool HasActionTag(const FGameplayTag& Tag);
};
