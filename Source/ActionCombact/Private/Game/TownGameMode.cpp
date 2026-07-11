// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TownGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/SlashHUD.h"
#include "Game/ActionGameInstance.h"

void ATownGameMode::BeginPlay()
{
	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PC->GetHUD());
	if (!SlashHUD) return;

	SlashHUD->SetTownHUD();
	SlashHUD->UpdateGoldWidget(GI->GetCurrentGold());
}
