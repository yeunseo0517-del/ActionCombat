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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void BindInventory(class UInventoryComponent* Inventory);

private:
	void ShowWidget(const struct FItemAddResult& Result, const FText& Name, const bool IsStackable);
	void HideWidget();
	void UpdateNotification(const FText& InName, const bool IsStackable, const int32 InQuantitiy, const FText& InAction);
	void SetTextsVisible(bool Value);

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> BoundInventory;

	FTimerHandle NotificationDestroyTimer;

	UPROPERTY(EditAnywhere, Category = Interaction)
	float NotiDestroyTime = 2.5f;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuantityPiece;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Quantity;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ActionText;
};
