// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"


#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Components/Attribute/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	TeamType = ETeamType::Enemy;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;
	if (EnemyState == EEnemyState::EES_Patrolling)
		CheckPatrolTarget();
	else
	{
		CheckCombatTarget();
	}

	if (CombatTarget && (IsEngaged() || IsAttacking()))
	{
		FaceTarget(CombatTarget);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

void AEnemy::GetHit(const FVector& ImpactPoint, UHitEffectDataAsset* HitEffectData, AActor* Hitter)
{
	if (IsDead()) return;
	EnterHitReact();
	Super::GetHit(ImpactPoint, HitEffectData, Hitter);
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::AttackEnd()
{
	if (IsDead()) return;
	SetEnemyState(EEnemyState::EES_NoState);
	CheckCombatTarget();
}

bool AEnemy::CanStartAttack()
{
	return !IsAttacking() && !IsDead() && !IsHitReacting();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitializeEnemy();
}

void AEnemy::OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsHitReactMontage(Montage))
	{
		SetEnemyState(EEnemyState::EES_NoState);
		CheckCombatTarget();
	}
}

void AEnemy::Die(const FName& Section)
{
	SetEnemyState(EEnemyState::EES_Dead);
	HideHealthBar();
	Super::Die(Section);
	ClearAttackTimer();
}

void AEnemy::SetEnemyState(EEnemyState NewState)
{
	EnemyState = NewState;
}

void AEnemy::CheckCombatTarget()
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
		if (EnemyState != EEnemyState::EES_Patrolling)
			StartPatrolling();
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartAttackTimer()
{
	SetEnemyState(EEnemyState::EES_Attacking);
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::BasicAttack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::StartPatrolling()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetEnemyState(EEnemyState::EES_Patrolling);
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && CanStartAttack() && !IsEngaged();
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsHitReacting()
{
	return EnemyState == EEnemyState::EES_HitReact;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::InTargetRange(AActor* Target, float Radius)
{
	if (!Target) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::BasicAttack()
{
	SetEnemyState(EEnemyState::EES_Engaged);
	Super::BasicAttack();
}

void AEnemy::EnterHitReact()
{
	SetEnemyState(EEnemyState::EES_HitReact);
	ShowHealthBar();
	ClearAttackTimer();
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(150.f);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::FaceTarget(AActor* Target)
{
	GetCharacterMovement()->bOrientRotationToMovement = false;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookAtRot, GetWorld()->GetDeltaSeconds(), 20.f));
}

void AEnemy::ChaseTarget()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetEnemyState(EEnemyState::EES_Chasing);
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

AActor* AEnemy::ChoosePatrolTarget()
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

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	bool bChaseTarget = !IsChasing()
		&& !IsDead()
		&& EnemyState < EEnemyState::EES_Attacking
		&& Cast<ABaseCharacter>(SeenPawn)->GetTeamType() == ETeamType::Player;
	if (bChaseTarget)
	{
		ClearPatrolTimer();
		CombatTarget = SeenPawn;
		ChaseTarget();
	}
}