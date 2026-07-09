// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Widgets/GateConfirmWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UGateConfirmWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UGateConfirmWidget::HandleConfirmClicked);
	if (CancelButton) CancelButton->OnClicked.AddDynamic(this, &UGateConfirmWidget::HandleCancelClicked);
}

void UGateConfirmWidget::HandleConfirmClicked()
{
	OnConfirm.Broadcast();
}

void UGateConfirmWidget::HandleCancelClicked()
{
	OnCancel.Broadcast();
}

void UGateConfirmWidget::SetMessage(const FText& MapNameText)
{
	if (MapName) MapName->SetText(MapNameText);
}