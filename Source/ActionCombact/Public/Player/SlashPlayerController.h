// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SlashPlayerController.generated.h"

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
};
