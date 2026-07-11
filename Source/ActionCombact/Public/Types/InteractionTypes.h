#pragma once

#include "CoreMinimal.h"
#include "InteractionTypes.generated.h"

UENUM()
enum class EInteractableType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NPC"),
	Device UMETA(DisplayName = "Device"),
	Store UMETA(DisplayName = "Store")
};

USTRUCT(BlueprintType)
struct FInteractableData
{
	GENERATED_BODY()

	FInteractableData()
		: InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		ActionText(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.f)
	{

	}

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText ActionText;

	UPROPERTY(EditInstanceOnly)
	int32 Quantity;

	UPROPERTY(EditInstanceOnly)
	bool bShowQuantity;

	UPROPERTY(EditInstanceOnly)
	float InteractionDuration = 0.f;
};