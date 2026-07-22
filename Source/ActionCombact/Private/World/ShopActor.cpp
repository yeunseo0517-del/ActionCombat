// Fill out your copyright notice in the Description page of Project Settings.


#include "World/ShopActor.h"
#include "Components/WidgetComponent.h"
#include "Player/SlashPlayerController.h"
#include "HUD/Interaction/InteractionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/ShopTypes.h"

AShopActor::AShopActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	ShopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shop Mesh"));
	SetRootComponent(ShopMesh);

	// Interaction Widget Component
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget Component"));
	InteractionWidgetComponent->SetupAttachment(GetRootComponent());
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawAtDesiredSize(true);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	InteractionWidgetComponent->SetPivot(FVector2D(0.f, 0.5f));
}

void AShopActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractionWidgetComponent)
	{
		if (UInteractionWidget* Widget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
		{
			Widget->UpdateWidget(InteractableData);
		}
	}
}

void AShopActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInFocus) UpdateWidgetPosition();
}

void AShopActor::BeginFocus()
{
	if (!ShopMesh) return;
	ShopMesh->SetRenderCustomDepth(true);

	bInFocus = true;
	if (InteractionWidgetComponent)
	{
		InteractionWidgetComponent->SetVisibility(true);
	}
}

void AShopActor::EndFocus()
{
	if (!ShopMesh) return;
	ShopMesh->SetRenderCustomDepth(false);

	bInFocus = false;
	if (InteractionWidgetComponent)
	{
		InteractionWidgetComponent->SetVisibility(false);
	}
}

void AShopActor::BeginInteract()
{
}

void AShopActor::EndInteract()
{
}

void AShopActor::Interact(AActor* Interactor)
{
	APawn* Player = Cast<APawn>(Interactor);
	if (!Player) return;

	ASlashPlayerController* PC = Cast<ASlashPlayerController>(Player->GetController());
	if (!PC) return;

	PC->OpenShop(this);
}

const FInteractableData& AShopActor::GetInteractableData() const
{
	return InteractableData;
}

TArray<FShopSlotData> AShopActor::GetShopItems()
{
	TArray<FShopSlotData> Result;
	for (const FShopEntry& Entry : ShopItems)
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(Entry.ItemID, Entry.ItemID.ToString());
		if (!ItemData) continue;

		FShopSlotData SlotData;
		SlotData.ItemID = ItemData->ItemID;
		SlotData.DisplayName = ItemData->ItemTextData.Name;
		SlotData.Icon = ItemData->ItemAssetData.Icon;
		SlotData.Price = ItemData->ItemStatistics.SellValue * Entry.PriceMultiplier;
		SlotData.Stock = Entry.Stock;
		SlotData.bCanPurchase = (Entry.Stock != 0) ? true : false;

		Result.Add(SlotData);
	}
	return Result;
}

void AShopActor::UpdateWidgetPosition()
{
	if (!InteractionWidgetComponent || !InteractionWidgetComponent->IsVisible()) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!CameraManager) return;

	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector CameraUp = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const FBoxSphereBounds Bounds = ShopMesh->Bounds;

	const FVector WidgetLocation = Bounds.Origin + CameraRight * (Bounds.SphereRadius + 20.f) + CameraUp * 20.f;

	InteractionWidgetComponent->SetWorldLocation(WidgetLocation);
}

void AShopActor::TryPurchase(const FName& TargetID)
{
	for (FShopEntry& Entry : ShopItems)
	{
		if (Entry.ItemID == TargetID)
		{
			Entry.Stock--;
			break;
		}
	}
}
