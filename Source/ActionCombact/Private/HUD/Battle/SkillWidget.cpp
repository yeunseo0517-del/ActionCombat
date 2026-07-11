// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/SkillWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Engine/Texture2D.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BaseImage && InIcon)
	{
		BaseImage->SetBrushFromTexture(InIcon);
		if (InUseBaseImage)
		{
			InUseBaseImage->SetBrushFromTexture(InIcon);
		}
	}

	UpdateUI();
}

void USkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bCheckRemainingCooldown)
	{
		float Remaining = EndTime - GetWorld()->GetTimeSeconds();
		if (Remaining <= 0.f)
		{
			bCheckRemainingCooldown = false;
			SetCooldown(false);
			return;
		}
		UpdateCooldownUI(Remaining);
	}
}

void USkillWidget::UpdateCooldownUI(float Remaining)
{
	if (UMaterialInstanceDynamic* Mat = CooldownOverlayImage->GetDynamicMaterial())
		Mat->SetScalarParameterValue(TEXT("Percent"), Remaining / CooldownDuration);

	CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(Remaining)));
}

void USkillWidget::SetInUse(bool bNew)
{
	bIsInUse = bNew;
	UpdateUI();
}

void USkillWidget::SetCooldown(bool bNew)
{
	bCooldown = bNew;
	UpdateUI();
}

void USkillWidget::CooldownStart(float InEndTime, float Cooldown)
{
	EndTime = InEndTime;
	CooldownDuration = Cooldown;
	bCheckRemainingCooldown = true;
	SetCooldown(true);
}

void USkillWidget::SkillActivated()
{
	SetInUse(true);
}

void USkillWidget::SkillDeactivated()
{
	SetInUse(false);
}

void USkillWidget::UpdateUI()
{
	InUseOverlay->SetVisibility(bIsInUse ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	CooldownOverlay->SetVisibility(bCooldown ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}