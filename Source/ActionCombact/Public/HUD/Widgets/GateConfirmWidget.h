// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GateConfirmWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGateTravelConfirm);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGateTravelCancel);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UGateConfirmWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintAssignable)
	FOnGateTravelConfirm OnConfirm;

	UPROPERTY(BlueprintAssignable)
	FOnGateTravelCancel OnCancel;

	void SetMessage(const FText& MapNameText);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MapName;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UFUNCTION()
	void HandleConfirmClicked();

	UFUNCTION()
	void HandleCancelClicked();
};
