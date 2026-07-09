// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;
class USkillHUDWidget;
class UResultWidget;
class UGateConfirmWidget;

UCLASS()
class ACTIONCOMBACT_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

public:
	void ShowBattleResult(const struct FBattleResult& Result);
	void ShowGateConfirmWidget(const FText& MapName, FSimpleDelegate OnConfirmed);
	void HideGateConfirmWidget();
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleGateConfirm();

	UFUNCTION()
	void HandleGateCancel();

	UFUNCTION()
	void CloseBattleResult();

	void SetUIOnlyInputMode();
	void SetGameAndUIInputMode();
	void RestoreGameInputMode();

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

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
	FORCEINLINE USkillHUDWidget* GetSkillHUD() const { return SkillHUD; }
	FORCEINLINE UResultWidget* GetResultWidget() const { return ResultWidget; }
	void SetTownHUD();
	void SetCombatHUD();
	void UpdateSlashOverlay(int32 Gold);
};
