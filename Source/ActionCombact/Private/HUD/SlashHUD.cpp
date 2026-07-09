// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "HUD/SkillHUDWidget.h"
#include "HUD/ResultWidget.h"
#include "HUD/Widgets/GateConfirmWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ActionGameInstance.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateWidgets();
	ApplyHUDMode();

	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->OnGoldChanged.AddDynamic(this, &ASlashHUD::UpdateGold);
	UpdateGold(GI->GetCurrentGold());
}

void ASlashHUD::CreateWidgets()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller)
		{
			if (SlashOverlayClass)
			{
				SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
				if (SlashOverlay)
				{
					SlashOverlay->AddToViewport();
					SlashOverlay->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
			if (SkillHUDClass)
			{
				SkillHUD = CreateWidget<USkillHUDWidget>(Controller, SkillHUDClass);
				if (SkillHUD)
				{
					SkillHUD->AddToViewport();
					SkillHUD->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
			if (ResultClass)
			{
				ResultWidget = CreateWidget<UResultWidget>(Controller, ResultClass);
				if (ResultWidget)
				{
					ResultWidget->AddToViewport();
					ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
					ResultWidget->OnCloseRequested.AddDynamic(this, &ASlashHUD::CloseBattleResult);
				}
			}
			if (GateConfirmClass)
			{
				GateConfirmWidget = CreateWidget<UGateConfirmWidget>(Controller, GateConfirmClass);
				if (GateConfirmWidget)
				{
					GateConfirmWidget->AddToViewport();
					GateConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
					GateConfirmWidget->OnConfirm.AddDynamic(this, &ASlashHUD::HandleGateConfirm);
					GateConfirmWidget->OnCancel.AddDynamic(this, &ASlashHUD::HandleGateCancel);
				}
			}
		}
	}
}

void ASlashHUD::ShowBattleResult(const FBattleResult& Result)
{
	if (!ResultWidget) return;

	ResultWidget->SetBattleResult(Result);
	ResultWidget->SetVisibility(ESlateVisibility::Visible);

	SetGameAndUIInputMode();
}

void ASlashHUD::CloseBattleResult()
{
	if (!ResultWidget) return;
	ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
	RestoreGameInputMode();
}

void ASlashHUD::UpdateGold(int32 Amount)
{
	if (!SlashOverlay) return;
	SlashOverlay->SetGold(Amount);
}

void ASlashHUD::ShowGateConfirmWidget(const FText& MapName, FSimpleDelegate OnConfirmed)
{
	if (!GateConfirmWidget) return;
	if(ResultWidget) ResultWidget->SetVisibility(ESlateVisibility::Collapsed);

	PendingGateConfirm = OnConfirmed;

	GateConfirmWidget->SetMessage(MapName);
	GateConfirmWidget->SetVisibility(ESlateVisibility::Visible);

	SetGameAndUIInputMode();
}

void ASlashHUD::HideGateConfirmWidget()
{
	if (!GateConfirmWidget) return;

	PendingGateConfirm.Unbind();

	GateConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);

	RestoreGameInputMode();
}

void ASlashHUD::HandleGateConfirm()
{
	FSimpleDelegate ConfirmToExecute = PendingGateConfirm;
	HideGateConfirmWidget();
	ConfirmToExecute.ExecuteIfBound();
}

void ASlashHUD::HandleGateCancel()
{
	HideGateConfirmWidget();
}

void ASlashHUD::SetUIOnlyInputMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	PC->SetShowMouseCursor(true);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ResultWidget->TakeWidget());
	PC->SetInputMode(InputMode);
}

void ASlashHUD::SetGameAndUIInputMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	PC->SetShowMouseCursor(true);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(GateConfirmWidget->TakeWidget());
	PC->SetInputMode(InputMode);
}

void ASlashHUD::RestoreGameInputMode()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	PC->SetShowMouseCursor(false);

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
}

void ASlashHUD::ApplyHUDMode()
{
	const bool bCombat = HUDMode == ESlashHUDMode::Combat;

	if (SlashOverlay) SlashOverlay->SetVisibility(ESlateVisibility::Visible);
	if (SkillHUD) SkillHUD->SetVisibility(bCombat ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (ResultWidget) ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
	if (GateConfirmWidget) GateConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ASlashHUD::SetTownHUD()
{
	HUDMode = ESlashHUDMode::Town;
}

void ASlashHUD::SetCombatHUD()
{
	HUDMode = ESlashHUDMode::Combat;
}

void ASlashHUD::UpdateSlashOverlay(int32 Gold)
{
	if (SlashOverlay) SlashOverlay->SetGold(Gold);
}
