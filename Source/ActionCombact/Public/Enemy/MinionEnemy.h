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
	virtual void UpdateMovement() override;
	virtual void TryAttack() override;
	virtual bool CanAttack() override;

private:
	void ChaseTarget();
	void InitializeEnemy();
	void UpdateBattleStrategy();
	bool IsOutsideAttackRadius();
	bool IsOutsideCombatRadius();
	bool IsPatrolling();
	void StartPatrolling();
	void CheckPatrolTarget();
	FVector ChoosePatrolTarget();
	void PatrolTimerFinished();
	void ClearPatrolTimer();
	void LoseInterest();
	void ReleaseSurroundSlot();
	void ShowHealthBar();
	void HideHealthBar();
	bool IsAtSlot();
	bool HasAssignedSlot();

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	/*
	AI Navigation
	*/
	FVector PatrolTarget;
	TArray<FVector> PatrolPoints;

	UPROPERTY(EditAnywhere)
	float PatrolRadius = 500.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f, PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f, ChasingSpeed = 800.f;;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	bool bShouldMoveLocation = false;
	FVector LastSlotLocation;
};
