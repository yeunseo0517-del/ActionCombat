// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Game/ActionGameInstance.h"
#include "Components/Attribute/AttributeComponent.h"
#include "HUD/SlashHUD.h"
#include "Game/Level/GateActor.h"
#include "Types/BattleMapConfig.h"

void ABattleGameMode::NotifyEnemyKilled(AActor* DeadEnemy)
{
	if (bBattleFinished) return;
	KilledEnemyCount++;
	CheckBattleEnd();
}

void ABattleGameMode::NotifyPlayerDead()
{
	if (bBattleFinished) return;
	FinishBattle(false);
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetBattleMapConfig();
	KilledEnemyCount = 0;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PC->GetHUD());
	if (!SlashHUD) return;

	SlashHUD->SetCombatHUD();
}

void ABattleGameMode::SetBattleMapConfig()
{
	FindBattleMapConfig();
	if (!BattleMapConfig) return;
	CurrentStageID = BattleMapConfig->CurrentStageID;
	TargetKillCount = BattleMapConfig->TargetKillCount;
	PortalSpawnPoint = BattleMapConfig->PortalSpawnPoint;
	TargetLevel = BattleMapConfig->TargetLevel;
	TargetLevelName = BattleMapConfig->TargetLevelName;
}

void ABattleGameMode::FindBattleMapConfig()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ABattleMapConfig::StaticClass(),
		FoundActors
	);

	if (FoundActors.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleMapConfig not found in this map."));
		return;
	}

	if (FoundActors.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Multiple BattleMapConfig actors found. Using first one."));
	}

	BattleMapConfig = Cast<ABattleMapConfig>(FoundActors[0]);

	if (!BattleMapConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast BattleMapConfig."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("BattleMapConfig loaded."));
}

void ABattleGameMode::CheckBattleEnd()
{
	if (KilledEnemyCount >= TargetKillCount) FinishBattle(true);
}

void ABattleGameMode::FinishBattle(bool bVictory)
{
	if (bBattleFinished) return;
	bBattleFinished = true;

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;

	CachedResult = MakeBattleResult(bVictory);

	if (CachedResult.bVictory)
	{
		GI->AddStageID(CachedResult.ClearedStageID);
	}

	GetWorldTimerManager().SetTimer(RewardCollectTimer, this, &ABattleGameMode::AfterRewardCollect, RewardCollectTime, false);
}

void ABattleGameMode::ShowBattleResultDelayed()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PC->GetHUD());
	if (SlashHUD) SlashHUD->ShowBattleResult(CachedResult);
}

void ABattleGameMode::SpawnPortalToTown()
{
	if (!ExitPortalClass || !PortalSpawnPoint) return;
	ExitPortal = GetWorld()->SpawnActor<AGateActor>(ExitPortalClass, PortalSpawnPoint->GetActorLocation(), PortalSpawnPoint->GetActorRotation());
	if (ExitPortal)
	{
		ExitPortal->SetDestination(TargetLevel, TargetLevelName);
	}
}

void ABattleGameMode::AfterRewardCollect()
{
	ShowBattleResultDelayed();
	SpawnPortalToTown();
}

FBattleResult ABattleGameMode::MakeBattleResult(bool InbVictory)
{
	FBattleResult Result;
	Result.bVictory = InbVictory;
	Result.ClearedStageID = CurrentStageID;
	Result.KilledEnemyCount = KilledEnemyCount;
	return Result;
}