// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameSave.h"


void UGameSave::AddGold(int32 Amount)
{
	ProfileData.Gold += Amount;
}

void UGameSave::AddStageID(int32 ID)
{
	if (!ProfileData.ClearedStageIDs.Contains(ID)) ProfileData.ClearedStageIDs.Add(ID);
}

int32 UGameSave::GetCurrentGold() const
{
	return ProfileData.Gold;
}