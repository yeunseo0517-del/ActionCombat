// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Types/ProfileData.h"
#include "ActionGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, const int32);

UCLASS()
class ACTIONCOMBACT_API UActionGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	FOnGoldChanged OnGoldChanged;
	
	void TravelToLevel(TSoftObjectPtr<UWorld> TargetLevel);
	void AddGoldData(int32 Amount);
	void AddStageID(int32 ID);

	int32 GetCurrentGold() const;

	void SaveProfile();
	
private:
	void LoadProfile();

	UPROPERTY()
	class UGameSave* GameSave;

	FString SaveSlotName = TEXT("GameSave");
};
