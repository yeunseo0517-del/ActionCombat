// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/Treasure.h"
#include "Components/Attribute/AttributeComponent.h"
#include "HUD/Battle/HealthBarComponent.h"
#include "Game/BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Types/Combat/HitContext.h"

AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bUseRVOAvoidance = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	GetCharacterMovement()->MaxAcceleration = 800.f;
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

void AEnemy::GetHit(const FHitInfo& HitInfo, UHitEffectDataAsset* HitEffectData, AActor* Hitter)
{
	if (IsDead()) return;
	Super::GetHit(HitInfo, HitEffectData, Hitter);
	EnterHitReact();
}

void AEnemy::AttackEnd()
{
	if (IsDead()) return;
	CurrentStateTag = FGameplayTag();
	const float ResumeTime = FMath::RandRange(MoveReMin, MoveReMax);
	GetWorldTimerManager().SetTimer(MoveResumeTimer, ResumeTime, false);
	StartAttackTimer();
}

bool AEnemy::CanStartAttack()
{
	return !IsAttacking() && !IsDead() && !IsHitReacting();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget) HealthBarWidget->BindAttribute(GetAttributeComponent());
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
	if (GetWorldTimerManager().IsTimerActive(AttackCooldownTimer)) return;

	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackCooldownTimer, AttackTime, false);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
}

bool AEnemy::CanAttack()
{
	return CanStartAttack() && !GetWorldTimerManager().IsTimerActive(AttackCooldownTimer);
}

bool AEnemy::IsEngaged()
{
	return CurrentStateTag == FGameplayTags::Get().State_AI_Engaged;
}

bool AEnemy::IsChasing()
{
	return CurrentStateTag == FGameplayTags::Get().State_AI_Chasing;
}

void AEnemy::EnterChaseState()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetCurrentState(FGameplayTags::Get().State_AI_Chasing);
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

bool AEnemy::CanMove()
{
	return !GetWorldTimerManager().IsTimerActive(MoveResumeTimer);
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}