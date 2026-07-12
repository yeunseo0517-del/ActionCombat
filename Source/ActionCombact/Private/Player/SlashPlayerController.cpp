// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SlashPlayerController.h"
#include "HUD/SlashHUD.h"

void ASlashPlayerController::ShowGateConfirm(const FText& MapName, FSimpleDelegate OnConfirmed)
{
	ASlashHUD* HUD = Cast<ASlashHUD>(GetHUD());
	if (!HUD) return;

	HUD->ShowGateConfirmWidget(MapName, OnConfirmed);
}

void ASlashPlayerController::HideGateConfirm()
{
	ASlashHUD* HUD = Cast<ASlashHUD>(GetHUD());
	if (!HUD) return;

	HUD->HideGateConfirmWidget();
}
