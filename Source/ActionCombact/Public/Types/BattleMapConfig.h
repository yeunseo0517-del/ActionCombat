// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleMapConfig.generated.h"

UCLASS()
class ACTIONCOMBACT_API ABattleMapConfig : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleMapConfig()
	{ }

	UPROPERTY(EditDefaultsOnly, Category = "DefaultSettings")
	int32 CurrentStageID = 1;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultSettings")
	int32 TargetKillCount = 1;

	UPROPERTY(EditInstanceOnly, Category = "AfterCombat")
	class ATargetPoint* PortalSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "AfterCombat")
	TSoftObjectPtr<UWorld> TargetLevel;

	UPROPERTY(EditAnywhere, Category = "AfterCombat")
	FText TargetLevelName;
};
