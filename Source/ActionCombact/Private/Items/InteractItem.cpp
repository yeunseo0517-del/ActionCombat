// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InteractItem.h"
#include "Components/WidgetComponent.h"
#include "HUD/InteractionWidget.h"
#include "Kismet/GameplayStatics.h"

AInteractItem::AInteractItem()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	InteractionWidgetComponent->SetupAttachment(GetRootComponent());

	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawAtDesiredSize(true);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	InteractionWidgetComponent->SetPivot(FVector2D(0.f, 0.5f));
}

void AInteractItem::Tick(float DeltaTime)
{
	if (ShouldUpdateWidgetPosition) UpdateWidgetPosition();
}

void AInteractItem::BeginFocus()
{
	if (!ItemMesh) return;
		ItemMesh->SetRenderCustomDepth(true);

	if (InteractionWidgetComponent)
	{
		ShouldUpdateWidgetPosition = true;
		InteractionWidgetComponent->SetVisibility(true);
	}
}

void AInteractItem::EndFocus()
{
	if (!ItemMesh) return;
	ItemMesh->SetRenderCustomDepth(false);

	if (InteractionWidgetComponent)
	{
		ShouldUpdateWidgetPosition = false;
		InteractionWidgetComponent->SetVisibility(false);
	}
}

void AInteractItem::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Interact"))
}

void AInteractItem::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("End Interact"))
}

void AInteractItem::Interact(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact"))
}

const FInteractableData& AInteractItem::GetInteractableData() const
{
	return InteractableData;
}

void AInteractItem::BeginPlay()
{
	Super::BeginPlay();

	if (UInteractionWidget* Widget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
	{
		Widget->UpdateWidget(InteractableData);
	}
}

void AInteractItem::UpdateWidgetPosition()
{
	if (!InteractionWidgetComponent || !InteractionWidgetComponent->IsVisible()) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!CameraManager) return;

	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRight =
		FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector CameraUp =
		FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const FBoxSphereBounds Bounds = ItemMesh->Bounds;

	const FVector WidgetLocation =
		Bounds.Origin
		+ CameraRight * (Bounds.SphereRadius + 20.f)
		+ CameraUp * 20.f;

	InteractionWidgetComponent->SetWorldLocation(WidgetLocation);
}
