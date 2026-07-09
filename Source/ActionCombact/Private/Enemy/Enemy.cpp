// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/Treasure.h"
#include "Components/Attribute/AttributeComponent.h"
#include "Game/BattleGameMode.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	TeamType = ETeamType::Enemy;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

void AEnemy::GetHit(const FVector& ImpactPoint, UHitEffectDataAsset* HitEffectData, AActor* Hitter)
{
	if (IsDead()) return;
	Super::GetHit(ImpactPoint, HitEffectData, Hitter);
	EnterHitReact();
}

void AEnemy::AttackEnd()
{
	if (IsDead()) return;
	CurrentStateTag = FGameplayTag();
	UpdateMovement();
}

bool AEnemy::CanStartAttack()
{
	return !IsAttacking() && !IsDead() && !IsHitReacting();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	SetCurrentState(FGameplayTag());

	if (IsHitReactMontage(Montage))
	{
		UpdateMovement();
	}
}

void AEnemy::Die(const FName& Section)
{
	if (EnemyController) EnemyController->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();
	Super::Die(Section);
	SpawnTreasure();
	ClearAttackTimer();

	ABattleGameMode* BattleGM = GetWorld()->GetAuthGameMode<ABattleGameMode>();

	if (BattleGM)
	{
		BattleGM->NotifyEnemyKilled(this);
	}
}

void AEnemy::SpawnTreasure()
{
	if (!GetWorld() || !TreasureClass) return;

	FVector Location = GetActorLocation();
	Location.Z += 50.f;
	ATreasure* Treaure = GetWorld()->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());

	if (Treaure && Attributes)
	{
		Treaure->SetGoldAmount(RewardGold);
	}
}

void AEnemy::StartAttackTimer()
{
	if (CombatTarget)
	{
		ABaseCharacter* Target = Cast<ABaseCharacter>(CombatTarget);
		if (!Target || Target->IsDead())
		{
			CombatTarget = nullptr;
			return;
		}
	}
	if (GetWorldTimerManager().IsTimerActive(AttackTimer)) return;

	if (EnemyController) EnemyController->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();

	SetCurrentState(FGameplayTags::Get().State_AI_Engaged);
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::TryAttack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::CanAttack()
{
	return CanStartAttack() && !IsEngaged();
}

bool AEnemy::IsEngaged()
{
	return CurrentStateTag == FGameplayTags::Get().State_AI_Engaged;
}

bool AEnemy::IsChasing()
{
	return CurrentStateTag == FGameplayTags::Get().State_AI_Chasing;
}

void AEnemy::ChaseTarget()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetCurrentState(FGameplayTags::Get().State_AI_Chasing);
	MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

bool AEnemy::InTargetRange(AActor* Target, float Radius)
{
	if (!Target) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

void AEnemy::EnterHitReact()
{
	if (IsDead()) return;
	if (EnemyController) EnemyController->StopMovement();
	GetCharacterMovement()->StopMovementImmediately();
	Super::EnterHitReact();
	ClearAttackTimer();
}

void AEnemy::FaceTarget()
{
	if (!CombatTarget) return;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
	LookAtRot.Pitch = 0.f;
	LookAtRot.Roll = 0.f;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookAtRot, GetWorld()->GetDeltaSeconds(), 20.f));
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}