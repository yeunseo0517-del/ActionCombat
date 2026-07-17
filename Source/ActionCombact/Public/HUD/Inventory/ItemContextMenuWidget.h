// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemContextMenuWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnItemRequested);

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UItemContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowMenuWidget(const FText& ActionText);

	FOnItemRequested OnActionClicked;
	FOnItemRequested OnDropClicked;

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleActionClicked();

	UFUNCTION()
	void HandleDropClicked();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* DropButton;
};
