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
class UShopWidget;

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

	void SetTownHUD();
	void SetCombatHUD();

	void BindInventory(class UInventoryComponent* Inventory);
	void BindAttribute(class UAttributeComponent* Attribute);

	void OpenShopWidget(class AShopActor* Shop);
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleGateConfirm();

	UFUNCTION()
	void HandleGateCancel();

	UFUNCTION()
	void CloseBattleResult();

	class ASlashPlayerController* GetSlashPC() const;
	void EnterGameAndUIMode(TSharedPtr<SWidget> InWidgetToFocus = nullptr);
	void ExitUIInputMode();
	void SetWidgetVisible(UUserWidget* Widget, bool bVisible);
	void CreateWidgets();
	void ApplyHUDMode();
	void HideAcquiredWidget();
	void ShowInventoryPanel();
	void HideInventoryPanel();

	template<typename TWidget>
	TWidget* CreateHUDWidget(TSubclassOf<TWidget> WidgetClass, ESlateVisibility Visibility = ESlateVisibility::Collapsed, int32 order = 0)
	{
		if (!PlayerOwner || !WidgetClass) return nullptr;

		TWidget* Widget = CreateWidget<TWidget>(PlayerOwner, WidgetClass);

		if (Widget)
		{
			Widget->AddToViewport(order);
			Widget->SetVisibility(Visibility);
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

	FSimpleDelegate PendingGateConfirm;

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

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<UShopWidget> ShopWidgetClass;

	UPROPERTY()
	UShopWidget* ShopWidget;
};
