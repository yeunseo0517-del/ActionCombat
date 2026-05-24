// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "HUD/HealthBar.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/Attribute/AttributeComponent.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement();
}

void ABossEnemy::UpdateMovement()
{
	if (IsDead() || !CombatTarget) return;
	if (CanAttack())
	{
		UpdateBattleStrategy();
	}
	if (!IsAttacking() && !IsHitReacting()) FaceTarget();
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	CreateHealthBarWidget();
	InitializeEnemy();
	InitializeSkills();
}

void ABossEnemy::CreateHealthBarWidget()
{
	if (HealthBarClass && !HealthBar)
	{
		HealthBar = CreateWidget<UHealthBar>(GetWorld(), HealthBarClass);
		if (HealthBar) HealthBar->AddToViewport();
	}
}

void ABossEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacter)
	{
		CombatTarget = PlayerCharacter;
		ChaseTarget();
	}
}

void ABossEnemy::InitializeSkills()
{
	RuntimeSkillPool.Empty();
	int SkillCount = 0;
	for (auto& It : SkillPool)
	{
		ESkillRange Range = It.Key;
		//UE_LOG(LogTemp, Warning, TEXT("Range Key: %d"), (int32)It.Key);
		//UE_LOG(LogTemp, Warning, TEXT("Entries Num: %d"), It.Value.Entries.Num());
		for (int32 i = 0; i < It.Value.Entries.Num(); ++i)
		{
			FSkillEntry& Entry = It.Value.Entries[i];
			USkillBase* RuntimeSkill = NewObject<USkillBase>(this, Entry.SkillClass);
			RuntimeSkill->Init(Entry, int32(Range), SkillCount);
			//UE_LOG(LogTemp, Warning, TEXT("Init Class: %s"), *GetNameSafe(Entry.SkillClass));
			FRuntimeSkillArray& Wrapper = RuntimeSkillPool.FindOrAdd(Range);
			Wrapper.Skills.Add(RuntimeSkill);
			SkillCount++;
		}
	}
}

USkillBase* ABossEnemy::SelectSkill(ESkillRange Range)
{
	FRuntimeSkillArray* Pool = RuntimeSkillPool.Find(Range);
	if (!Pool || Pool->Skills.IsEmpty()) return nullptr;
	TArray<USkillBase*> AvailableSkills;
	for (USkillBase* Skill : Pool->Skills)
	{
		if (!Skill->IsSkillOnCooldown()) AvailableSkills.Add(Skill);
	}
	if (AvailableSkills.Num() == 0) return nullptr;

	int32 RandomVal = FMath::RandRange(0, AvailableSkills.Num() - 1);
	return AvailableSkills[RandomVal];
}

void ABossEnemy::UpdateBattleStrategy()
{
	if (!CanAttack()) return;
	SelectedSkill = nullptr;

	float Distance = FVector::Dist(GetActorLocation(), CombatTarget->GetActorLocation());
	if (Distance > FarRange)
	{
		SelectedSkill = SelectSkill(ESkillRange::Far);
	}
	else
	{
		SelectedSkill = SelectSkill(ESkillRange::Mid);
	}

	if (SelectedSkill || IsInsideAttackRadius())
	{
		StartAttackTimer();
	}
	else
	{
		ChaseTarget();
	}
}

void ABossEnemy::TryAttack()
{
	if (SelectedSkill)
	{
		if (Combat) Combat->SetCurrentSkill(SelectedSkill);
		SelectedSkill->ActivateSkill(this);
	}
	else
	{
		Attack(FGameplayTags::Get().Action_Attack_Basic);
	}
}

void ABossEnemy::EnterHitReact()
{
	if (IsDead()) return;
	if (HealthBar)
	{
		HealthBar->SetHealthPercent(Attributes->GetHealthPercent());
	}
	Super::EnterHitReact();
}
