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
}

void ASlashHUD::CreateWidgets()
{
	if (!SlashOverlay) SlashOverlay = CreateHUDWidget<USlashOverlay>(SlashOverlayClass);
	if (SlashOverlay) SetWidgetVisible(SlashOverlay, true);

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

	if (!AcquiredNotification) AcquiredNotification = CreateHUDWidget<UAcquiredNotificationWidget>(AcquiredNotificationClass);
	if (AcquiredNotification) SetWidgetVisible(AcquiredNotification, true);

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
		if (!InventoryPanelClass) return;
		InventoryPanel = CreateHUDWidget<UInventoryPanelWidget>(InventoryPanelClass);
		if (!InventoryPanel) return;
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

void ASlashHUD::BindInventory(UInventoryComponent* Inventory)
{
	if (!AcquiredNotification) AcquiredNotification = CreateHUDWidget<UAcquiredNotificationWidget>(AcquiredNotificationClass);
	if (!AcquiredNotification) return;
	AcquiredNotification->BindInventory(Inventory);
}

void ASlashHUD::BindAttribute(UAttributeComponent* Attribute)
{
	if (!SlashOverlay) SlashOverlay = CreateHUDWidget<USlashOverlay>(SlashOverlayClass);
	if (!SlashOverlay) return;
	SlashOverlay->BindAttribute(Attribute);
}