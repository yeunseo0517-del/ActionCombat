// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionEnemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Components/Attribute/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"

AMinionEnemy::AMinionEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void AMinionEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
}

void AMinionEnemy::UpdateMovement()
{
	if (IsDead()) return;
	if (IsPatrolling())
		CheckPatrolTarget();
	else
	{
		UpdateBattleStrategy();
	}

	bool bHavetoFaceTarget = IsEngaged() || IsAttacking();
	if (CombatTarget && bHavetoFaceTarget)
	{
		FaceTarget();
	}
}

float AMinionEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMinionEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AMinionEnemy::PawnSeen);
	InitializeEnemy();
}

void AMinionEnemy::Die(const FName& Section)
{
	HideHealthBar();
	Super::Die(Section);
}

void AMinionEnemy::EnterHitReact()
{
	if (IsDead()) return;
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	ShowHealthBar();
	Super::EnterHitReact();
}

bool AMinionEnemy::CanAttack()
{
	return IsInsideAttackRadius() && Super::CanAttack();
}

void AMinionEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	SetCurrentState(FGameplayTags::Get().State_AI_Patrolling);
	MoveToTarget(PatrolTarget);
	HideHealthBar();
}

void AMinionEnemy::UpdateBattleStrategy()
{
	if (CombatTarget)
	{
		if (IsOutsideCombatRadius())
		{
			ClearAttackTimer();
			LoseInterest();
			if (!IsEngaged())
			{
				StartPatrolling();
			}
		}
		else if (IsOutsideAttackRadius() && !IsChasing())
		{
			ClearAttackTimer();
			if (!IsEngaged()) ChaseTarget();
		}
		else if (CanAttack())
		{
			StartAttackTimer();
		}
	}
	else
	{
		StopMontage();
		ClearAttackTimer();
		if (!IsPatrolling())
			StartPatrolling();
	}
}

void AMinionEnemy::TryAttack()
{
	if (!CanStartAttack()) return;
	Attack(FGameplayTags::Get().Action_Attack_Basic);
}

void AMinionEnemy::ChaseTarget()
{
	Super::ChaseTarget();
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
}

bool AMinionEnemy::IsPatrolling()
{
	return CurrentStateTag == FGameplayTags::Get().State_AI_Patrolling;
}

void AMinionEnemy::StartPatrolling()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetCurrentState(FGameplayTags::Get().State_AI_Patrolling);
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AMinionEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AMinionEnemy::PatrolTimerFinished, WaitTime);
	}
}

AActor* AMinionEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	if (PatrolTarget)
	{
		for (AActor* Actor : PatrolTargets)
			if (Actor != PatrolTarget)
				ValidTargets.AddUnique(Actor);
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AMinionEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AMinionEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

bool AMinionEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AMinionEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

void AMinionEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AMinionEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AMinionEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AMinionEnemy::PawnSeen(APawn* SeenPawn)
{
	bool bIsEnemy = (TeamType != Cast<ABaseCharacter>(SeenPawn)->GetTeamType());
	bool bCanChase = IsUnoccupied() || IsPatrolling();
	if (bIsEnemy && bCanChase)
	{
		ClearPatrolTimer();
		CombatTarget = SeenPawn;
		ChaseTarget();
	}
}