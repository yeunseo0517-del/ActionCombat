// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TownGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ATownGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

protected:
	virtual void BeginPlay() override;
};
