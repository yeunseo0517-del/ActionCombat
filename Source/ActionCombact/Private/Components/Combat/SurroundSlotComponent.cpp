#include "Components/Combat/SurroundSlotComponent.h"

// Sets default values for this component's properties
USurroundSlotComponent::USurroundSlotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int32 USurroundSlotComponent::RequestSlot(AActor* Requester)
{
	FVector Dir = (Requester->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	FVector LocalDir = GetOwner()->GetActorTransform().InverseTransformVectorNoScale(Dir);
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(LocalDir.Y, LocalDir.X));
	int32 CenterIndex = FMath::RoundToInt(Angle / (360.f / SlotCount));
	for (int32 Step = 0; Step < SlotCount; ++Step)
	{
		int32 Offset = (Step + 1) / 2 * (Step % 2 ? 1 : -1);
		int32 Index = (CenterIndex + Offset + SlotCount) % SlotCount;
		if (!SurroundSlots[Index].Occupant.IsValid())
		{
			SurroundSlots[Index].Occupant = Requester;
			return Index;
		}
	}
	return INDEX_NONE;
}

FVector USurroundSlotComponent::GetSlotWorldLocation(int32 Index)
{
	FVector WorldOffset = GetOwner()->GetActorTransform().TransformVectorNoScale(SurroundSlots[Index].Direction) * Radius;
	FVector WorldLocation = GetOwner()->GetActorLocation() + WorldOffset;
	DrawDebugSphere(GetWorld(), WorldLocation, 12, 8, FColor::Red, false, 2.f);
	return WorldLocation;
}

void USurroundSlotComponent::ReleaseSlot(int32 Index)
{
	SurroundSlots[Index].Occupant = nullptr;
}

void USurroundSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	SurroundSlots.Empty();
	for (int32 i = 0; i < SlotCount; ++i)
	{
		float AngleRad = FMath::DegreesToRadians((360.f / SlotCount) * i);
		FSurroundSlot Slot;
		Slot.Direction = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);
		SurroundSlots.Add(Slot);
	}
}

void USurroundSlotComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	DrawDebugCircle(GetWorld(), GetOwner()->GetActorLocation(), Radius, 64, FColor::Green, false, 5.f, 0, 2.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), false);
}

