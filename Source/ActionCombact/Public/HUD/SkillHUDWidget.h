// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SkillTypes.h"
#include "SkillHUDWidget.generated.h"

class USkillWidget;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API USkillHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	USkillWidget* Slot_Q;

	UPROPERTY(meta = (BindWidget))
	USkillWidget* Slot_E;

	UPROPERTY(meta = (BindWidget))
	USkillWidget* Slot_R;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleSkillActivated(ESkillSlot InSlot);

	UFUNCTION()
	void HandleCooldownStart(ESkillSlot InSlot, float EndTime, float SkillDuration);

	UFUNCTION()
	void HandleSkillDeactivated(ESkillSlot InSlot);
};
