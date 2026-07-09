// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/Game/BattleResult.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void NotifyEnemyKilled(AActor* DeadEnemy);
	void NotifyPlayerDead();

protected:
	virtual void BeginPlay() override;

private:
	void SetBattleMapConfig();
	void FindBattleMapConfig();
	void CheckBattleEnd();
	void FinishBattle(bool bVictory);
	void ShowBattleResultDelayed();
	void SpawnPortalToTown();
	void AfterRewardCollect();
	FBattleResult MakeBattleResult(bool bVictory);
	int32 GetPlayerEarnedGold() const;

	UPROPERTY()
	class ABattleMapConfig* BattleMapConfig;

	int32 CurrentStageID = 1;
	int32 KilledEnemyCount = 0;
	int32 TargetKillCount = 1;
	bool bBattleFinished = false;

	FTimerHandle ResultTimer;
	FTimerHandle RewardCollectTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Battle")
	float RewardCollectTime = 5.f;

	UPROPERTY()
	FBattleResult CachedResult;

	UPROPERTY(EditDefaultsOnly, Category = "AfterCombat")
	TSubclassOf<class AGateActor> ExitPortalClass;

	UPROPERTY()
	class AGateActor* ExitPortal;

	UPROPERTY()
	class ATargetPoint* PortalSpawnPoint;

	UPROPERTY()
	TSoftObjectPtr<UWorld> TargetLevel;

	FText TargetLevelName;
};
