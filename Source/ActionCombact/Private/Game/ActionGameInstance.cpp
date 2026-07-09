// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ActionGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/GameSave.h"

void UActionGameInstance::Init()
{
	Super::Init();
	LoadProfile();
}

void UActionGameInstance::TravelToLevel(TSoftObjectPtr<UWorld> TargetLevel)
{
	SaveProfile();
	if (TargetLevel.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetLevel is not set."));
		return;
	}
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetLevel);
}

void UActionGameInstance::AddGoldData(int32 Amount)
{
	if (!GameSave) return;
	GameSave->AddGold(Amount);
	OnGoldChanged.Broadcast(GetCurrentGold());
}

void UActionGameInstance::AddStageID(int32 ID)
{
	if (!GameSave) return;
	GameSave->AddStageID(ID);
}

int32 UActionGameInstance::GetCurrentGold() const
{
	if (!GameSave) return 0;
	return GameSave->GetCurrentGold();
}

void UActionGameInstance::LoadProfile()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		GameSave = Cast<UGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	if(!GameSave) GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
}

void UActionGameInstance::SaveProfile()
{
	if (!GameSave)
	{
		GameSave = Cast<UGameSave>(UGameplayStatics::CreateSaveGameObject(UGameSave::StaticClass()));
	}

	if (!GameSave) return;
	UGameplayStatics::SaveGameToSlot(GameSave, SaveSlotName, 0);
}
