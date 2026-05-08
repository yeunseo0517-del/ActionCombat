// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SkillHUDWidget.h"
#include "HUD/SkillWidget.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void USkillHUDWidget::NativeConstruct()
{
	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (IStatusReceiverInterface* Receiver = Cast<IStatusReceiverInterface>(OwningPawn))
		{
			UStatusComponent* StatusComp = Receiver->GetStatusComponent();
			if (StatusComp)
			{
				StatusComp->OnSkillActivated.AddDynamic(this, &USkillHUDWidget::HandleSkillActivated);
				StatusComp->OnCooldownStart.AddDynamic(this, &USkillHUDWidget::HandleCooldownStart);
				StatusComp->OnSkillDeactivated.AddDynamic(this, &USkillHUDWidget::HandleSkillDeactivated);
			}
		}
	}
}

void USkillHUDWidget::HandleSkillActivated(ESkillSlot InSlot)
{
	if (InSlot == ESkillSlot::Q && Slot_Q) Slot_Q->SkillActivated();
	if (InSlot == ESkillSlot::E && Slot_E) Slot_E->SkillActivated();
	if (InSlot == ESkillSlot::R && Slot_R) Slot_R->SkillActivated();
}

void USkillHUDWidget::HandleCooldownStart(ESkillSlot InSlot, float EndTime, float SkillDuration)
{
	if (InSlot == ESkillSlot::Q && Slot_Q) Slot_Q->CooldownStart(EndTime, SkillDuration);
	if (InSlot == ESkillSlot::E && Slot_E) Slot_E->CooldownStart(EndTime, SkillDuration);
	if (InSlot == ESkillSlot::R && Slot_R) Slot_R->CooldownStart(EndTime, SkillDuration);
}

void USkillHUDWidget::HandleSkillDeactivated(ESkillSlot InSlot)
{
	if (InSlot == ESkillSlot::Q && Slot_Q) Slot_Q->SkillDeactivated();
	if (InSlot == ESkillSlot::E && Slot_E) Slot_E->SkillDeactivated();
	if (InSlot == ESkillSlot::R && Slot_R) Slot_R->SkillDeactivated();
}
