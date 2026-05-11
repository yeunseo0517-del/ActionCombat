// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SkillHUDWidget.h"
#include "HUD/SkillWidget.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void USkillHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<USkillWidget*> TempSlots = { Slot_Q, Slot_E, Slot_R };
	for (int32 i = 0; i < TempSlots.Num(); ++i)
	{
		if (TempSlots[i]) SlotMap.Add(i, TempSlots[i]);
	}

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

void USkillHUDWidget::HandleSkillActivated(int32 InSlotKey)
{
	if (USkillWidget* Widget = SlotMap.FindRef(InSlotKey))
	{
		Widget->SkillActivated();
	}
}

void USkillHUDWidget::HandleCooldownStart(int32 InSlotKey, float EndTime, float SkillDuration)
{
	if (USkillWidget* Widget = SlotMap.FindRef(InSlotKey))
	{
		Widget->CooldownStart(EndTime, SkillDuration);
	}
}

void USkillHUDWidget::HandleSkillDeactivated(int32 InSlotKey)
{
	if (USkillWidget* Widget = SlotMap.FindRef(InSlotKey))
	{
		Widget->SkillDeactivated();
	}
}
