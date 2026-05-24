// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;

UCLASS()
class ACTIONCOMBACT_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	// AActor
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Hit Interface function
	virtual void GetHit(const FVector& ImpactPoint, class UHitEffectDataAsset* HitEffectData, AActor* Hitter) override;
	//

	virtual void AttackEnd() override;
	virtual bool CanStartAttack() override;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void Die(const FName& Section) override;
	virtual void EnterHitReact() override;
	virtual void UpdateMovement() {}
	virtual void UpdateBattleStrategy() {}

	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, float Radius);
	bool IsInsideAttackRadius();
	void StartAttackTimer();
	void ClearAttackTimer();
	void FaceTarget();
	bool IsEngaged();
	bool IsChasing();
	virtual void ChaseTarget();
	virtual bool CanAttack();
	virtual void TryAttack() {}

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(VisibleInstanceOnly)
	AActor* CombatTarget;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f, AttackMax = 1.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 300.f;

	UPROPERTY(EditAnywhere)
	double AcceptanceRadius = 150.f;
};
