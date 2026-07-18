// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/ProfileData.h"
#include "GameFramework/SaveGame.h"
#include "GameSave.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UGameSave : public USaveGame
{
	GENERATED_BODY()

private:
	UPROPERTY()
	FProfileData ProfileData;

public:
	void AddGold(int32 Amount);
	void AddStageID(int32 StageID);
	int32 GetCurrentGold() const;
	FProfileData& GetProfileData() { return ProfileData; }
};
