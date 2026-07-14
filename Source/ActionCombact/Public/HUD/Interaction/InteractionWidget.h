// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(const struct FInteractableData& InteractableData);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ActionText;
};
