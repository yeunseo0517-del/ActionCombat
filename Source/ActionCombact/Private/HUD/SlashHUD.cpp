// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "HUD/SkillHUDWidget.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller)
		{
			if (SlashOverlayClass)
			{
				SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
				if (SlashOverlay) SlashOverlay->AddToViewport();
			}
			if (SkillHUDClass)
			{
				SkillHUD = CreateWidget<USkillHUDWidget>(Controller, SkillHUDClass);
				if (SkillHUD) SkillHUD->AddToViewport();
			}
		}
	}
}