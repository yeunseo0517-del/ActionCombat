// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SkillTypes.h"
#include "SkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SkillActivated();
	void SkillDeactivated();
	void CooldownStart(float EndTime, float Duration);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* BaseImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* InUseBaseImage;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* InUseOverlay;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* CooldownOverlay;

	UPROPERTY(meta = (BindWidget))
	class UImage* CooldownOverlayImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* InIcon;

private:
	void UpdateVisibility();
	void UpdateUI();
	void UpdateCooldownUI(float Remaining);

	float EndTime;
	float CooldownDuration;
	bool bCheckRemainingCooldown;
	bool bIsInUse = false;
	bool bCooldown = false;

	void SetInUse(bool bNew);
	void SetCooldown(bool bNew);
};
