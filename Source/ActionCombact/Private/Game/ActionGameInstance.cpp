// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ActionGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/GameSave.h"

void UActionGameInstance::Init()
{
	Super::Init();
	LoadProfile();
}

void UActionGameInstance::AddGoldData(int32 Amount)
{
	ProfileData.Gold += Amount;
}

void UActionGameInstance::AddStageID(int32 ID)
{
	if(!ProfileData.ClearedStageIDs.Contains(ID)) ProfileData.ClearedStageIDs.Add(ID);
}

int32 UActionGameInstance::GetCurrentGold() const
{
	return ProfileData.Gold;
}

void UActionGameInstance::LoadProfile()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		GameSave = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

		if (GameSave)
		{
			ProfileData = GameSave->GetSaveProfileData();
			return;
		}
	}

	GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
	ProfileData = FProfileData();
}

void UActionGameInstance::SaveProfile()
{
	if (!GameSave)
	{
		GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
	}

	if (!GameSave) return;
	GameSave->SetSaveProfileData(ProfileData);
	UGameplayStatics::SaveGameToSlot(GameSave, SaveSlotName, 0);
}
