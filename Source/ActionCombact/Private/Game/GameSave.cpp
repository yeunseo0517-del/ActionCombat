// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameSave.h"

void UGameSave::SetSaveProfileData(const FProfileData& Data)
{
	ProfileData = Data;
}

FProfileData UGameSave::GetSaveProfileData() const
{
	return ProfileData;
}
