// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/BaseCharacter.h"
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

	virtual void BasicAttack() override;
	virtual void AttackEnd() override;
	virtual bool CanStartAttack() override;
	

protected:
	virtual void BeginPlay() override;

	virtual void OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void Die(const FName& Section) override;

private:
	/*
		State
	*/
	UPROPERTY(VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	void SetEnemyState(EEnemyState NewState);

	// AI Behavior
	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void StartAttackTimer();
	void ClearAttackTimer();
	void StartPatrolling();
	void ShowHealthBar();
	void HideHealthBar();
	void LoseInterest();
	bool CanAttack();
	bool IsAttacking();
	bool IsEngaged();
	bool IsChasing();
	bool IsHitReacting();
	bool IsDead();
	bool InTargetRange(AActor* Target, float Radius);
	bool IsOutsideAttackRadius();
	bool IsOutsideCombatRadius();
	bool IsInsideAttackRadius();
	void EnterHitReact();
	void MoveToTarget(AActor* Target);
	void FaceTarget(AActor* Target);
	void ChaseTarget();
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent


	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(VisibleInstanceOnly)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 300, CombatRadius = 500;

	/*
		AI Navigation
	*/
	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 250.f;

	/*
		Timer
	*/
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f, PatrolWaitMax = 10.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f, AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f, ChasingSpeed = 300.f;
};
