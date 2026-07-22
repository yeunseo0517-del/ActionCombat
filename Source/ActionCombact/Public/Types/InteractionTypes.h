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

	UPROPERTY(EditAnywhere)
	EInteractableType InteractableType;

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText ActionText;

	UPROPERTY(EditAnywhere)
	int32 Quantity;

	UPROPERTY(EditAnywhere)
	bool bShowQuantity;

	UPROPERTY(EditAnywhere)
	float InteractionDuration = 0.f;
};