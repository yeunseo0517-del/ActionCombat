// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Types/ProfileData.h"
#include "ActionGameInstance.generated.h"

/**

SaveGame에서 ProfileData를 불러옴
런타임 중 ProfileData를 들고 있음
골드/재료/강화 단계 변경
필요한 시점에 SaveGame으로 저장
맵 이동 처리

 */
UCLASS()
class ACTIONCOMBACT_API UActionGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void AddGoldData(int32 Amount);
	void AddStageID(int32 ID);

	int32 GetCurrentGold() const;

	void SaveProfile();
	
private:
	void LoadProfile();

	UPROPERTY()
	FProfileData ProfileData;

	UPROPERTY()
	class UGameSave* GameSave;

	FString SaveSlotName = TEXT("GameSave");
};
