// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InteractItem.h"
#include "Items/ItemBase.h"
#include "Types/Item/ItemDataStructs.h"
#include "Types/Item/ItemAddResult.h"
#include "Components/WidgetComponent.h"
#include "HUD/Interaction/InteractionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/LootReceiverInterface.h"

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
		UpdateWidgetPosition();
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
	//UE_LOG(LogTemp, Warning, TEXT("Begin Interact"))
}

void AInteractItem::EndInteract()
{
	//UE_LOG(LogTemp, Warning, TEXT("End Interact"))
}

void AInteractItem::Interact(AActor* Interactor)
{
	if (!IsPendingKillPending())
	{
		if (ItemInstance)
		{
			if (ILootReceiverInterface* Player = Cast<ILootReceiverInterface>(Interactor))
			{
				const FItemAddResult& Result = Player->AddItem(ItemInstance);
				switch (Result.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;
				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					ItemInstance->SetQuantity(ItemInstance->GetQuantity() - Result.ActualAmountAdded);
					UpdateInteractableData();
					RefreshInteractionWidget();
					break;
				default:
					break;
				}
			}
		}
	}
}

void AInteractItem::RefreshInteractionWidget()
{
	if (!InteractionWidgetComponent || !InteractionWidgetComponent->IsVisible()) return;
	if (UInteractionWidget* Widget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
	{
		Widget->UpdateWidget(InteractableData);
	}
}


const FInteractableData& AInteractItem::GetInteractableData() const
{
	return InteractableData;
}

void AInteractItem::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(ItemQauntity);

	if (UInteractionWidget* Widget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
	{
		Widget->UpdateWidget(InteractableData);
	}
}

void AInteractItem::InitializePickup(const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());
		if (!ItemData) return;

		ItemInstance = NewObject<UItemBase>(this);
		if (ItemData->ItemNumericData.bIsStackable) InteractableData.bShowQuantity = true;
		ItemInstance->SetItemData(*ItemData, InQuantity <= 0? 1 : InQuantity);
		ItemMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);

		UpdateInteractableData();
	}
}

void AInteractItem::InitializeDrop(const UItemBase* ItemToDrop, const int32 InQuantity)
{
	if (!ItemToDrop || InQuantity <= 0) return;

	ItemInstance = ItemToDrop->CreateItemCopy();
	ItemInstance->SetQuantity(InQuantity);

	const FItemData& Data = ItemInstance->GetItemData();
	ItemMesh->SetStaticMesh(Data.ItemAssetData.Mesh);

	UpdateInteractableData();
}

void AInteractItem::UpdateInteractableData()
{
	if (!ItemInstance) return;
	const FItemData& Data = ItemInstance->GetItemData();

	InteractableData.InteractableType = EInteractableType::Pickup;
	InteractableData.ActionText = Data.ItemTextData.InteractionText;
	InteractableData.Name = Data.ItemTextData.Name;
	InteractableData.Quantity = ItemInstance->GetQuantity();
}


void AInteractItem::UpdateWidgetPosition()
{
	if (!InteractionWidgetComponent || !InteractionWidgetComponent->IsVisible()) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!CameraManager) return;

	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector CameraUp = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const FBoxSphereBounds Bounds = ItemMesh->Bounds;

	const FVector WidgetLocation = Bounds.Origin + CameraRight * (Bounds.SphereRadius + 20.f) + CameraUp * 20.f;

	InteractionWidgetComponent->SetWorldLocation(WidgetLocation);
}

void AInteractItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AInteractItem, DesiredItemID))
	{
		if (ItemDataTable)
		{
			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
			{
				ItemMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);
			}
		}
	}
}
