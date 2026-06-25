// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;
class USkillHUDWidget;


UCLASS()
class ACTIONCOMBACT_API ASlashHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USkillHUDWidget> SkillHUDClass;

	UPROPERTY()
	USkillHUDWidget* SkillHUD;

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
	FORCEINLINE USkillHUDWidget* GetSkillHUD() const { return SkillHUD; }
};
