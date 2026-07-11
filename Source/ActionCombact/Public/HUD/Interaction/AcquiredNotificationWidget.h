// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AcquiredNotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UAcquiredNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateNotification(const struct FInteractableData& Data);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuantityPiece;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Quantity;
};
