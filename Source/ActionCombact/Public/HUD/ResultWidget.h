// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Game/BattleResult.h"
#include "ResultWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResultCloseRequested);

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBattleResult(const FBattleResult& InBattleResult);

	UPROPERTY(BlueprintAssignable)
	FOnResultCloseRequested OnCloseRequested;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleCloseButtonClicked();
	
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Victory;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KilledEnemy;
};
