// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SlashPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API ASlashPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void ShowGateConfirm(const FText& MapName, FSimpleDelegate OnConfirmed);
	void HideGateConfirm();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

private:
	void InitializeHUD();
	void ToggleInventory();

	UPROPERTY()
	class ASlashHUD* HUD;

	/*
	Enchanced Input
	*/
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SlashCharacterContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ToggleInventoryAction;
};
