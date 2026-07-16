// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SlashPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "HUD/SlashHUD.h"
#include "Characters/SlashCharacter.h"

void ASlashPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(SlashCharacterContext, 0);
	}
	InitializeHUD();
}

void ASlashPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitializeHUD();
}

void ASlashPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ASlashPlayerController::ToggleInventory);
	}
}

void ASlashPlayerController::InitializeHUD()
{
	if (!HUD) HUD = Cast<ASlashHUD>(GetHUD());

	if (ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetPawn()))
	{
		HUD->BindInventory(SlashCharacter->GetInventoryComponent());
		HUD->BindAttribute(SlashCharacter->GetAttributeComponent());
	}
}

void ASlashPlayerController::ToggleInventory()
{
	if (!HUD) HUD = Cast<ASlashHUD>(GetHUD());
	HUD->ToggleInventory();
}

void ASlashPlayerController::ShowGateConfirm(const FText& MapName, FSimpleDelegate OnConfirmed)
{
	if (!HUD) HUD = Cast<ASlashHUD>(GetHUD());
	HUD->ShowGateConfirmWidget(MapName, OnConfirmed);
}

void ASlashPlayerController::HideGateConfirm()
{
	if (!HUD) HUD = Cast<ASlashHUD>(GetHUD());
	HUD->HideGateConfirmWidget();
}