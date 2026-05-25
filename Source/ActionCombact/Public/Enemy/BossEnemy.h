// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "BossEnemy.generated.h"

USTRUCT(BlueprintType)
struct FSkillArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillEntry> Entries;
};

USTRUCT(BlueprintType)
struct FRuntimeSkillArray
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<USkillBase*> Skills;
};

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:
	ABossEnemy();

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	void InitializeEnemy();
	virtual void UpdateMovement() override;
	virtual void UpdateBattleStrategy() override;
	virtual void TryAttack() override;
	virtual void EnterHitReact() override;

private:
	void CreateHealthBarWidget();
	void InitializeSkills();
	USkillBase* SelectSkill(ESkillRange Range);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UHealthBar> HealthBarClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	UHealthBar* HealthBar;

	UPROPERTY(EditAnywhere)
	double FarRange = 1500.f, MidRange = 800.f;

	// Àû ½ºÅ³
	UPROPERTY(EditAnywhere, Category = "Skills")
	TMap<ESkillRange, FSkillArrayWrapper> SkillPool;

	UPROPERTY(VisibleAnywhere, Category = "Skills")
	TMap<ESkillRange, FRuntimeSkillArray> RuntimeSkillPool;

	UPROPERTY(VisibleAnywhere, Category = "Skills")
	USkillBase* SelectedSkill;
};
