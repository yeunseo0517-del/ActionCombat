#include "Components/Combat/SurroundSlotComponent.h"

// Sets default values for this component's properties
USurroundSlotComponent::USurroundSlotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USurroundSlotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAssignSlots)
	{
		bAssignSlots = false;
		AssignSlots();
	}
}

void USurroundSlotComponent::AssignSlots()
{
	SlotScores.Empty();

	const float DistanceWeight = 0.01f;
	for (int32 EnemyIndex = 0; EnemyIndex < PendingEnemies.Num(); ++EnemyIndex)
	{
		for (int32 SlotIndex = 0; SlotIndex < FreeSlotIndices.Num(); ++SlotIndex)
		{
			FEnemySlotScore Score;
			Score.Enemy = PendingEnemies[EnemyIndex];
			Score.SlotIndex = FreeSlotIndices[SlotIndex];

			if (!Score.Enemy.IsValid()) continue;
			FVector Dir = (Score.Enemy->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
			float EnemyAngle = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.X));
			float SlotAngle = SurroundSlots[Score.SlotIndex].Angle;
			float AngleCost = FMath::Abs(FMath::FindDeltaAngleDegrees(EnemyAngle, SlotAngle));
			float DistanceCost = FVector::Dist2D(Score.Enemy->GetActorLocation(), GetSlotWorldLocation(Score.SlotIndex));
			Score.Cost = AngleCost + DistanceCost * DistanceWeight;

			SlotScores.Add(Score);
		}
	}

	SlotScores.Sort([](const FEnemySlotScore& A, const FEnemySlotScore& B)
		{
			return A.Cost < B.Cost;
		});

	for (int32 Step = 0; Step < SlotScores.Num(); ++Step)
	{
		int32 Index = SlotScores[Step].SlotIndex;
		TWeakObjectPtr<AActor> Enemy = SlotScores[Step].Enemy;
		if (PendingEnemies.Find(Enemy) == INDEX_NONE || SurroundSlots[Index].Occupant.IsValid()) continue;
		SurroundSlots[Index].Occupant = Enemy;
		PendingEnemies.Remove(Enemy);
		FreeSlotIndices.Remove(Index);
	}
}

int32 USurroundSlotComponent::FindAssignedSlotIndex(AActor* Requester) const
{
	for (int32 Index = 0; Index < SurroundSlots.Num(); ++Index)
	{
		if (SurroundSlots[Index].Occupant == Requester) return Index;
	}
	return INDEX_NONE;
}

void USurroundSlotComponent::RequestSlot(AActor* Requester)
{
	if (!Requester || PendingEnemies.Contains(Requester) || FindAssignedSlotIndex(Requester) != INDEX_NONE) return;
	PendingEnemies.AddUnique(Requester);
	bAssignSlots = true;
}

bool USurroundSlotComponent::GetAssignedSlotLocation(AActor* Requester, FVector& OutLocation) const
{
	const int32 Index = FindAssignedSlotIndex(Requester);
	if (Index == INDEX_NONE) return false;

	OutLocation = GetSlotWorldLocation(Index);
	return true;
}

FVector USurroundSlotComponent::GetSlotWorldLocation(int32 Index) const
{
	FVector WorldLocation = GetOwner()->GetActorLocation() + SurroundSlots[Index].Direction * Radius;
	//DrawDebugSphere(GetWorld(), WorldLocation, 12, 8, FColor::Red, false, 2.f);
	return WorldLocation;
}

void USurroundSlotComponent::ReleaseSlot(AActor* Requester)
{
	if (!Requester) return;

	PendingEnemies.Remove(Requester);

	const int32 Index = FindAssignedSlotIndex(Requester);
	if (Index == INDEX_NONE) return;

	SurroundSlots[Index].Occupant = nullptr;
	FreeSlotIndices.AddUnique(Index);

	if (!PendingEnemies.IsEmpty()) bAssignSlots = true;
}

//bool USurroundSlotComponent::CanKeepSlot(AActor* Requester)
//{
//	const int32 Index = FindAssignedSlotIndex(Requester);
//	if (Index == INDEX_NONE) return false;
//
//	const float SlotAngle = SurroundSlots[Index].Angle;
//	const float ReleaseAngleThreshold = (360.f / SlotCount) * 0.5f;
//
//	FVector Dir = (Requester->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
//	const float RequesterAngle = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.X));
//
//	const float AngleDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(SlotAngle, RequesterAngle));
//
//	if(AngleDelta > ReleaseAngleThreshold)
//	{
//		ReleaseSlot(Requester);
//		RequestSlot(Requester);
//		return false;
//	}
//	return true;
//}

void USurroundSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	SurroundSlots.Empty();
	for (int32 i = 0; i < SlotCount; ++i)
	{
		FSurroundSlot Slot;
		Slot.Angle = (360.f / SlotCount) * i;
		float AngleRad = FMath::DegreesToRadians(Slot.Angle);
		Slot.Direction = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);
		SurroundSlots.Add(Slot);
		FreeSlotIndices.Add(i);
	}
}

void USurroundSlotComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	DrawDebugCircle(GetWorld(), GetOwner()->GetActorLocation(), Radius, 64, FColor::Green, false, 5.f, 0, 2.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), false);
}
