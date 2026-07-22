// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/Battle/SlashOverlay.h"
#include "HUD/Battle/SkillHUDWidget.h"
#include "HUD/Battle/ResultWidget.h"
#include "HUD/Interaction/GateConfirmWidget.h"
#include "HUD/Interaction/InteractionWidget.h"
#include "HUD/Interaction/AcquiredNotificationWidget.h"
#include "HUD/Inventory/InventoryPanelWidget.h"
#include "HUD/Shop/ShopWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Game/ActionGameInstance.h"
#include "Player/SlashPlayerController.h"
#include "Interfaces/InteractableInterface.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateWidgets();
	ApplyHUDMode();
}

void ASlashHUD::CreateWidgets()
{
	if (!SlashOverlay) SlashOverlay = CreateHUDWidget<USlashOverlay>(SlashOverlayClass, ESlateVisibility::Visible);

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
	if (!AcquiredNotification) AcquiredNotification = CreateHUDWidget<UAcquiredNotificationWidget>(AcquiredNotificationClass, ESlateVisibility::Visible);
	if (!InventoryPanel) InventoryPanel = CreateHUDWidget<UInventoryPanelWidget>(InventoryPanelClass, ESlateVisibility::Collapsed, 100);
}

void ASlashHUD::SetTownHUD()
{
	HUDMode = ESlashHUDMode::Town;
}

void ASlashHUD::SetCombatHUD()
{
	HUDMode = ESlashHUDMode::Combat;
}

void ASlashHUD::ApplyHUDMode()
{
	const bool bCombat = HUDMode == ESlashHUDMode::Combat;

	if (SlashOverlay) SetWidgetVisible(SlashOverlay, true);
	if (SkillHUD) SetWidgetVisible(SkillHUD, bCombat);
}

void ASlashHUD::ShowBattleResult(const FBattleResult& Result)
{
	if (!ResultWidget) return;

	ResultWidget->SetBattleResult(Result);
	SetWidgetVisible(ResultWidget, true);
	EnterGameAndUIMode(ResultWidget->TakeWidget());
}

void ASlashHUD::CloseBattleResult()
{
	if (!ResultWidget) return;
	SetWidgetVisible(ResultWidget, false);
	ExitUIInputMode();
}

void ASlashHUD::ShowGateConfirmWidget(const FText& MapName, FSimpleDelegate OnConfirmed)
{
	if (!GateConfirmWidget) return;
	if (ResultWidget) SetWidgetVisible(ResultWidget, false);

	PendingGateConfirm = OnConfirmed;

	GateConfirmWidget->SetMessage(MapName);
	SetWidgetVisible(GateConfirmWidget, true);
	EnterGameAndUIMode(GateConfirmWidget->TakeWidget());
}

void ASlashHUD::HideGateConfirmWidget()
{
	if (!GateConfirmWidget) return;

	PendingGateConfirm.Unbind();

	SetWidgetVisible(GateConfirmWidget, false);

	ExitUIInputMode();
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
		ShowInventoryPanel();
	}
	else
	{
		HideInventoryPanel();
	}
}

void ASlashHUD::ShowInventoryPanel()
{
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	InventoryPanel->SetDesiredSizeInViewport(FVector2D(800.f, 600.f));
	InventoryPanel->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	InventoryPanel->SetPositionInViewport(ViewportSize * 0.5f, true);
	SetWidgetVisible(InventoryPanel, true);
	EnterGameAndUIMode(InventoryPanel->TakeWidget());
}

void ASlashHUD::HideInventoryPanel()
{
	InventoryPanel->HideChildWidgets();
	SetWidgetVisible(InventoryPanel, false);
	ExitUIInputMode();
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

ASlashPlayerController* ASlashHUD::GetSlashPC() const
{
	return Cast<ASlashPlayerController>(GetOwningPlayerController());
}

void ASlashHUD::EnterGameAndUIMode(TSharedPtr<SWidget> InWidgetToFocus = nullptr)
{
	if (ASlashPlayerController* PC = GetSlashPC()) PC->SetGameAndUIInputMode(InWidgetToFocus);
}

void ASlashHUD::ExitUIInputMode()
{
	if (ASlashPlayerController* PC = GetSlashPC()) PC->RestoreGameInputMode();
}

void ASlashHUD::SetWidgetVisible(UUserWidget* Widget, bool bVisible)
{
	if (!Widget) return;
	Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ASlashHUD::BindInventory(UInventoryComponent* Inventory)
{
	if (!AcquiredNotification) AcquiredNotification = CreateHUDWidget<UAcquiredNotificationWidget>(AcquiredNotificationClass);
	if (AcquiredNotification)
	{
		AcquiredNotification->BindInventory(Inventory);
	}

	if (!InventoryPanel) InventoryPanel = CreateHUDWidget<UInventoryPanelWidget>(InventoryPanelClass, ESlateVisibility::Collapsed, 100);
	if (InventoryPanel)
	{
		InventoryPanel->BindInventory(Inventory);
	}
}

void ASlashHUD::BindAttribute(UAttributeComponent* Attribute)
{
	if (!SlashOverlay) SlashOverlay = CreateHUDWidget<USlashOverlay>(SlashOverlayClass);
	if (SlashOverlay)
	{
		SlashOverlay->BindAttribute(Attribute);
	}
}

void ASlashHUD::OpenShopWidget(AShopActor* Shop)
{
	ShopWidget = CreateHUDWidget<UShopWidget>(ShopWidgetClass, ESlateVisibility::Visible);
	if (!ShopWidget) return;

	ShopWidget->BindShop(Shop);
}