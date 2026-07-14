// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/Battle/SlashOverlay.h"
#include "HUD/Battle/SkillHUDWidget.h"
#include "HUD/Battle/ResultWidget.h"
#include "HUD/Interaction/GateConfirmWidget.h"
#include "HUD/Interaction/InteractionWidget.h"
#include "HUD/Interaction/AcquiredNotificationWidget.h"
#include "HUD/Inventory/InventoryPanelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ActionGameInstance.h"
#include "Interfaces/InteractableInterface.h"

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
	if(!SlashOverlay) SlashOverlay = CreateHUDWidget<USlashOverlay>(SlashOverlayClass);
	if(!SkillHUD) SkillHUD = CreateHUDWidget<USkillHUDWidget>(SkillHUDClass);
	if (!ResultWidget)
	{
		ResultWidget = CreateHUDWidget<UResultWidget>(ResultClass);
		if (ResultWidget) ResultWidget->OnCloseRequested.AddDynamic(this, &ASlashHUD::CloseBattleResult);
	}

	if (!GateConfirmWidget)
	{
		GateConfirmWidget = CreateHUDWidget<UGateConfirmWidget>(GateConfirmClass);
		if (GateConfirmWidget)
		{
			GateConfirmWidget->OnConfirm.AddDynamic(this, &ASlashHUD::HandleGateConfirm);
			GateConfirmWidget->OnCancel.AddDynamic(this, &ASlashHUD::HandleGateCancel);
		}
	}

	if(!InteractionWidget) InteractionWidget = CreateHUDWidget<UInteractionWidget>(InteractionClass);
	if (!AcquiredNotification)
	{
		AcquiredNotification = CreateHUDWidget<UAcquiredNotificationWidget>(AcquiredNotificationClass);
		if (AcquiredNotification)
		{
			AcquiredNotification->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if(!InventoryPanel) InventoryPanel = CreateHUDWidget<UInventoryPanelWidget>(InventoryPanelClass);
}

void ASlashHUD::ShowBattleResult(const FBattleResult& Result)
{
	if (!ResultWidget) return;

	ResultWidget->SetBattleResult(Result);
	SetWidgetVisible(ResultWidget, true);

	SetGameAndUIInputMode();
}

void ASlashHUD::CloseBattleResult()
{
	if (!ResultWidget) return;
	SetWidgetVisible(ResultWidget, false);
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
	if (ResultWidget) SetWidgetVisible(ResultWidget, false);

	PendingGateConfirm = OnConfirmed;

	GateConfirmWidget->SetMessage(MapName);
	SetWidgetVisible(GateConfirmWidget, true);

	SetGameAndUIInputMode();
}

void ASlashHUD::HideGateConfirmWidget()
{
	if (!GateConfirmWidget) return;

	PendingGateConfirm.Unbind();

	SetWidgetVisible(GateConfirmWidget, false);

	RestoreGameInputMode();
}

void ASlashHUD::HideAcquiredWidget()
{
	if (!AcquiredNotification) return;
	SetWidgetVisible(AcquiredNotification, false);
}

void ASlashHUD::ShowInteractionWidget(const FInteractableData& InteractableData)
{
	if (!InteractionWidget) return;
	if (!InteractionWidget->IsVisible())
	{
		SetWidgetVisible(InteractionWidget, true);
	}

	//InteractionWidget->UpdateWidget(InteractableData);
}

void ASlashHUD::ToggleInventory()
{
	if (!InventoryPanel)
	{
		if (InventoryPanelClass) InventoryPanel = CreateHUDWidget<UInventoryPanelWidget>(InventoryPanelClass);
	}

	if (!InventoryPanel->IsVisible())
	{
		SetWidgetVisible(InventoryPanel, true);
		SetGameAndUIInputMode();
	}
	else
	{
		SetWidgetVisible(InventoryPanel, false);
		RestoreGameInputMode();
	}
}

void ASlashHUD::SetHealth(float current, float max)
{
	SlashOverlay->SetHealthPercent(current, max);
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

void ASlashHUD::SetWidgetVisible(UUserWidget* Widget, bool bVisible)
{
	if (!Widget) return;
	Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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

	if (SlashOverlay) SetWidgetVisible(SlashOverlay, true);
	if (SkillHUD)
	{
		SetWidgetVisible(SkillHUD, bCombat);
	}
}

void ASlashHUD::SetTownHUD()
{
	HUDMode = ESlashHUDMode::Town;
}

void ASlashHUD::SetCombatHUD()
{
	HUDMode = ESlashHUDMode::Combat;
}

void ASlashHUD::UpdateGoldWidget(int32 Gold)
{
	if (SlashOverlay) SlashOverlay->SetGold(Gold);
}

void ASlashHUD::BindInventory(UInventoryComponent* Inventory) const
{
	if (AcquiredNotification) AcquiredNotification->BindInventory(Inventory);
}