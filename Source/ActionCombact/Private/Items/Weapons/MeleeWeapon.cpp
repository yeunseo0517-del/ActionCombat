// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/MeleeWeapon.h"
#include "Items/Weapons/Data/CombatDataAsset.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Interfaces/CombatInterface.h"

AMeleeWeapon::AMeleeWeapon()
{
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AMeleeWeapon::DoTrace()
{
	if (!CurrentTraceData)
	{
		UE_LOG(LogTemp, Error, TEXT("Please check Character's Combat Component and assign the CombatDataAsset"));
		return;
	}

	TArray<AActor*> ActorsToIgnore = BuildActorsToIgnore();
	TSet<AActor*> AlreadyHit;
	TArray<FHitResult> HitResults;

	switch (CurrentTraceData->Shape)
	{
	case ETraceType::ETT_Sphere:
	case ETraceType::ETT_BoxSweep:
		ExecuteSweepTrace(ActorsToIgnore, HitResults);
		break;
	case ETraceType::ETT_BoxSingle:
		DoBoxTraceMulti(GetTraceStart(), GetTraceEnd(), ActorsToIgnore, HitResults);
		break;
	default:
		break;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AlreadyHit.Add(Hit.GetActor());
	}

	CurHitContext = BuildWeaponHitContext(ActorsToIgnore, AlreadyHit);

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetInstigator()))
	{
		CombatInterface->GetCombatComponent()->ProcessHitResults(HitResults);
	}
}


void AMeleeWeapon::ExecuteSweepTrace(const TArray<AActor*>& InActorsToIgnore, TArray<FHitResult>& HitResults)
{
	FVector CurrentStart = GetTraceStart();
	FVector CurrentEnd = GetTraceEnd();
	FVector CurrentCenter = (CurrentStart + CurrentEnd) * .5f;

	if (!bHasPrevLocation)
	{
		UpdatePrevLocations(CurrentStart, CurrentEnd, CurrentCenter);
		bHasPrevLocation = true;
		return;
	}

	switch (CurrentTraceData->Shape)
	{
	case ETraceType::ETT_Sphere:
		DoSphereTrace(PrevStartLocation, CurrentStart, CurrentTraceData->Radius, InActorsToIgnore, HitResults);
		break;
	case ETraceType::ETT_BoxSweep:
		DoBoxTraceMulti(PrevCenter, CurrentCenter, InActorsToIgnore, HitResults);
		break;
	default:
		break;
	}

	UpdatePrevLocations(CurrentStart, CurrentEnd, CurrentCenter);
}

void AMeleeWeapon::UpdatePrevLocations(const FVector& Start, const FVector& End, const FVector& Center)
{
	PrevStartLocation = Start;
	PrevEndLocation = End;
	PrevCenter = Center;
}

FRotator AMeleeWeapon::GetRotation()
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	USceneComponent* Mesh = ItemMesh;
	if (CanGetTrace() && bUseCharacterSocket)
	{
		Mesh = CombatInterface->GetCombatMesh();
	}
	return Mesh ? Mesh->GetSocketRotation(CurrentTraceData->Steps[CurrentTraceIndex].StartSocket) : FRotator::ZeroRotator;
}

void AMeleeWeapon::DoBoxTraceMulti(const FVector Start, const FVector End, const TArray<AActor*>& InActorsToIgnore, TArray<FHitResult>& BoxHits)
{
	FRotator TraceRotation = GetRotation();
	FVector DefaultHalfSize = CurrentTraceData->HalfSize;
	FVector BoxExtent = DefaultHalfSize;

	if (CurrentTraceData->Shape == ETraceType::ETT_BoxSweep)
	{
		float Length = FVector::Dist(Start, End);
		BoxExtent = FVector(DefaultHalfSize.X, DefaultHalfSize.Y, Length * .5f);
	}

	UKismetSystemLibrary::BoxTraceMulti(
		this,
		Start,
		End,
		BoxExtent,
		TraceRotation,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		InActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHits,
		true
	);
}

void AMeleeWeapon::DoSphereTrace(const FVector PrevLoc, const FVector CurLoc, float InRadius, const TArray<AActor*>& InActorsToIgnore, TArray<FHitResult>& SphereHits)
{
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		PrevLoc,
		CurLoc,
		InRadius,
		TraceTypeQuery1,
		false,
		InActorsToIgnore,
		EDrawDebugTrace::None,
		SphereHits,
		true
	);
}

FVector AMeleeWeapon::GetTrace(FName SocketName) const
{
	USceneComponent* Mesh = ItemMesh;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		if (bUseCharacterSocket && CombatInterface->GetCombatMesh())
		{
			Mesh = CombatInterface->GetCombatMesh();
		}
	}
	return Mesh ? Mesh->GetSocketLocation(SocketName) : GetActorLocation();
}

bool AMeleeWeapon::CanGetTrace() const
{
	return CurrentTraceData && CurrentTraceData->Steps.IsValidIndex(CurrentTraceIndex);
}

FVector AMeleeWeapon::GetTraceStart() const
{
	if (CanGetTrace())
	{
		return GetTrace(CurrentTraceData->Steps[CurrentTraceIndex].StartSocket);
	}
	else
		return GetActorLocation();
}

FVector AMeleeWeapon::GetTraceEnd() const
{
	if (CanGetTrace())
		return GetTrace(CurrentTraceData->Steps[CurrentTraceIndex].EndSocket);
	else
		return GetActorLocation();
}

FName AMeleeWeapon::GetTraceStartName()
{
	if (CanGetTrace())
		return CurrentTraceData->Steps[CurrentTraceIndex].StartSocket;
	else
		return FName();
}

void AMeleeWeapon::SetCombatTraceData(const FCombatTraceData* NewData, const FGameplayTag& Tag)
{
	if (NewData)
	{
		CurrentTraceData = NewData;
	}
	else
	{
		if (WeaponData)
		{
			CurrentTraceData = WeaponData->AttackSet.Find(Tag);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Fail to Set Current Trace Data"));
	}
}

