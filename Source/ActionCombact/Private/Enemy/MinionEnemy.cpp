// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionEnemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

#include "Components/Attribute/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/Battle/HealthBarComponent.h"
#include "Components/Combat/SurroundSlotComponent.h"

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
	if (bShouldMoveLocation && CombatTarget)
	{
		if (USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>())
		{
			FVector CurrentSlotLocation;
			if (!SlotComp->GetAssignedSlotLocation(this, CurrentSlotLocation))
			{
				bShouldMoveLocation = false;
			}
			else
			{
				const float MoveThreshold = FMath::Square(10.f);

				if (FVector::DistSquared2D(CurrentSlotLocation, LastSlotLocation) > MoveThreshold)
				{
					if (AAIController* AIController = Cast<AAIController>(GetController()))
					{
						AIController->MoveToLocation(CurrentSlotLocation, 40.f, false);
						LastSlotLocation = CurrentSlotLocation;
					}
				}
			}
		}
	}
	UpdateMovement();
}

void AMinionEnemy::UpdateMovement()
{
	if (IsDead()) return;
	if (IsPatrolling())
		CheckPatrolTarget();
	else if(CanMove())
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
	ClearPatrolTimer();
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
	ClearPatrolTimer();
	LoseInterest();
	Super::Die(Section);
}

void AMinionEnemy::EnterHitReact()
{
	if (IsDead()) return;
	ShowHealthBar();
	ClearPatrolTimer();
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
	const int32 PatrolPointCount = FMath::RandRange(MinPatrolPoint, MaxPatrolPoint);
	for (int32 Step = 0; Step < PatrolPointCount; ++Step)
	{
		FVector RandVector(FMath::RandRange(0.f, PatrolRadius), FMath::RandRange(0.f, PatrolRadius), 0.f);
		PatrolPoints.AddUnique(GetActorLocation() + RandVector);
		//DrawDebugSphere(GetWorld(), GetActorLocation() + RandVector, 12.f, 8, FColor::Blue, true);
	}
	PatrolTarget = PatrolPoints[0];
	if(EnemyController) EnemyController->MoveToLocation(PatrolTarget, 40.f, false);
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
		else if (CanAttack())
		{
			TryAttack();
		}
		else if (HasAssignedSlot() && !IsAttacking() && !IsAtSlot())
		{
			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				if (AIController->GetMoveStatus() != EPathFollowingStatus::Moving)
				{
					if (USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>())
					{
						FVector SlotLocation;
						if (SlotComp->GetAssignedSlotLocation(this, SlotLocation))
						{
							AIController->MoveToLocation(SlotLocation, 40.f, false);
							LastSlotLocation = SlotLocation;
							bShouldMoveLocation = true;
						}
					}
				}
			}
		}
		else if (!IsAttacking() && !IsHitReacting() && (!HasAssignedSlot() || !IsChasing()))
		{
			ChaseTarget();
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
	if (EnemyController) EnemyController->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bShouldMoveLocation = false;
	Attack(FGameplayTags::Get().Action_Attack_Basic);
}

void AMinionEnemy::ChaseTarget()
{
	if (!CombatTarget) return;
	ClearPatrolTimer();
	EnterChaseState();
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;

	if (HasAssignedSlot()) return;
	USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>();
	if (!SlotComp) return;
	SlotComp->RequestSlot(this);

	FVector CurrentSlotLocation;
	if (SlotComp->GetAssignedSlotLocation(this, CurrentSlotLocation))
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->MoveToLocation(CurrentSlotLocation, 40.f, false);
		}
		bShouldMoveLocation = true;
	}
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
	if(EnemyController) EnemyController->MoveToLocation(PatrolTarget, 40.f, false);
}

void AMinionEnemy::CheckPatrolTarget()
{
	if (GetWorldTimerManager().IsTimerActive(PatrolTimer)) return;
	if (FVector::DistSquared(GetActorLocation(), PatrolTarget) <= FMath::Square(50.f))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AMinionEnemy::PatrolTimerFinished, WaitTime);
	}
}

FVector AMinionEnemy::ChoosePatrolTarget()
{
	TArray<FVector> ValidPoints;
	for (const FVector& Point : PatrolPoints)
	{
		if (Point != PatrolTarget) ValidPoints.Add(Point);
	}
	const int32 NumPatrolPoints = ValidPoints.Num();
	if (NumPatrolPoints > 0) return ValidPoints[FMath::RandRange(0, NumPatrolPoints - 1)];

	return FVector::ZeroVector;
}

void AMinionEnemy::PatrolTimerFinished()
{
	if (!IsPatrolling()) return;
	if (EnemyController) EnemyController->MoveToLocation(PatrolTarget, 40.f, false);
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
	ReleaseSurroundSlot();
	bShouldMoveLocation = false;
}

void AMinionEnemy::ReleaseSurroundSlot()
{
	if (CombatTarget && HasAssignedSlot())
	{
		USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>();
		if (SlotComp)
		{
			SlotComp->ReleaseSlot(this);
		}
	}
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

bool AMinionEnemy::IsAtSlot()
{
	if (!HasAssignedSlot() || !CombatTarget) return false;
	USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>();
	if (!SlotComp) return false;

	FVector SlotLocation;
	if (!SlotComp->GetAssignedSlotLocation(this, SlotLocation)) return false;
	return FVector::DistSquared(GetActorLocation(), SlotLocation) <= FMath::Square(50.f);
}

bool AMinionEnemy::HasAssignedSlot()
{
	USurroundSlotComponent* SlotComp = CombatTarget->FindComponentByClass<USurroundSlotComponent>();
	if (!SlotComp) return false;

	FVector Location;
	return SlotComp->GetAssignedSlotLocation(this, Location);
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