// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "MinionEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API AMinionEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	AMinionEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void Die(const FName& Section) override;
	virtual void EnterHitReact() override;
	virtual void UpdateBattleStrategy() override;
	virtual void ChaseTarget() override;
	virtual bool CanAttack() override;

private:
	void InitializeEnemy();
	void UpdateMovement();
	bool IsOutsideAttackRadius();
	bool IsOutsideCombatRadius();
	bool IsPatrolling();
	void StartPatrolling();
	void CheckPatrolTarget();
	AActor* ChoosePatrolTarget();
	void PatrolTimerFinished();
	void ClearPatrolTimer();
	void LoseInterest();
	void ShowHealthBar();
	void HideHealthBar();

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	/*
	AI Navigation
	*/
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 250.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f, PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f, ChasingSpeed = 300.f;;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent
};
