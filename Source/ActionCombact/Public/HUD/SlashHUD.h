// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

UENUM()
enum class ESlashHUDMode : uint8
{
	Town,
	Combat
};

class USlashOverlay;
class USkillHUDWidget;
class UResultWidget;
class UGateConfirmWidget;
class UInteractionWidget;
class UInventoryPanelWidget;
class UAcquiredNotificationWidget;

UCLASS()
class ACTIONCOMBACT_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

public:
	void ShowBattleResult(const struct FBattleResult& Result);
	void ShowGateConfirmWidget(const FText& MapName, FSimpleDelegate OnConfirmed);
	void HideGateConfirmWidget();
	void ShowInteractionWidget(const struct FInteractableData& InteractableData);
	void ToggleInventory();

	void SetHealth(float current, float max);
	void SetTownHUD();
	void SetCombatHUD();
	void UpdateGoldWidget(int32 Gold);

	void BindInventory(class UInventoryComponent* Inventory) const;
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleGateConfirm();

	UFUNCTION()
	void HandleGateCancel();

	UFUNCTION()
	void CloseBattleResult();

	UFUNCTION()
	void UpdateGold(int32 Amount);

	void SetWidgetVisible(UUserWidget* Widget, bool bVisible);
	void SetUIOnlyInputMode();
	void SetGameAndUIInputMode();
	void RestoreGameInputMode();
	void CreateWidgets();
	void ApplyHUDMode();
	void HideAcquiredWidget();

	template<typename TWidget>
	TWidget* CreateHUDWidget(TSubclassOf<TWidget> WidgetClass)
	{
		if (!PlayerOwner || !WidgetClass) return nullptr;

		TWidget* Widget = CreateWidget<TWidget>(PlayerOwner, WidgetClass);

		if (Widget)
		{
			Widget->AddToViewport();
			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}
		return Widget;
	}

	ESlashHUDMode HUDMode = ESlashHUDMode::Town;

	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly, Category = Skill)
	TSubclassOf<USkillHUDWidget> SkillHUDClass;

	UPROPERTY()
	USkillHUDWidget* SkillHUD;

	UPROPERTY(EditDefaultsOnly, Category = Result)
	TSubclassOf<UResultWidget> ResultClass;

	UPROPERTY()
	UResultWidget* ResultWidget;

	UPROPERTY(EditDefaultsOnly, Category = Gate)
	TSubclassOf<UGateConfirmWidget> GateConfirmClass;

	UPROPERTY()
	UGateConfirmWidget* GateConfirmWidget;

	UPROPERTY(EditDefaultsOnly, Category = Interaction)
	TSubclassOf<UInteractionWidget> InteractionClass;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY(EditDefaultsOnly, Category = Interaction)
	TSubclassOf<UAcquiredNotificationWidget> AcquiredNotificationClass;

	UPROPERTY()
	UAcquiredNotificationWidget* AcquiredNotification;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<UInventoryPanelWidget> InventoryPanelClass;

	UPROPERTY()
	UInventoryPanelWidget* InventoryPanel;

	FSimpleDelegate PendingGateConfirm;
};
