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

	TArray<AActor*> ActorsToIgnore = MakeActorsToIgnore();
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

	for (FHitResult Hit : HitResults)
	{
		AlreadyHit.Add(Hit.GetActor());
	}

	CurHitContext = MakeHitContext(ActorsToIgnore, AlreadyHit);

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetInstigator()))
	{
		CombatInterface->GetCombatComponent()->ProcessHitResults(HitResults);
	}
}

FHitContext AMeleeWeapon::MakeHitContext(const TArray<AActor*>& Ignore, const TSet<AActor*>& AlreadyHit)
{
	FHitContext HitContext;

	HitContext.Instigator = GetInstigator() ? GetInstigator() : Cast<APawn>(GetOwner());
	HitContext.DamageCauser = this;
	HitContext.AttackTag = CurrentTraceData->AttackTag;
	HitContext.Damage = WeaponData->DefaultDamage;
	HitContext.ActorsToIgnore = Ignore;

	return HitContext;
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
	if (CombatInterface && CurrentTraceData->Steps.IsValidIndex(CurrentTraceIndex))
	{
		return bUseCharacterSocket ? CombatInterface->GetCombatMesh()->GetSocketRotation(CurrentTraceData->Steps[CurrentTraceIndex].StartSocket)
			: ItemMesh->GetSocketRotation(CurrentTraceData->Steps[CurrentTraceIndex].StartSocket);
	}
	else
		return FRotator::ZeroRotator;
}

void AMeleeWeapon::DoBoxTraceMulti(const FVector Start, const FVector End, const TArray<AActor*>& InActorsToIgnore, TArray<FHitResult>& BoxHits)
{
	FRotator TraceRotation = GetRotation();
	FVector BoxExtent;
	FVector DefaultHalfSize = CurrentTraceData->HalfSize;

	if (CurrentTraceData->Shape == ETraceType::ETT_BoxSweep)
	{
		float Length = FVector::Dist(Start, End);
		BoxExtent = FVector(DefaultHalfSize.X, DefaultHalfSize.Y, Length * .5f);
	}
	else
	{
		BoxExtent = DefaultHalfSize;
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

TArray<AActor*> AMeleeWeapon::MakeActorsToIgnore()
{
	TArray<AActor*> Actors;
	Actors.AddUnique(this);
	Actors.AddUnique(GetOwner());
	return Actors;
}

FVector AMeleeWeapon::GetTrace(FName SocketName) const
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		if (bUseCharacterSocket && CombatInterface->GetCombatMesh())
		{
			return CombatInterface->GetCombatMesh()->GetSocketLocation(SocketName);
		}
		else
		{
			return ItemMesh->GetSocketLocation(SocketName);
		}
	}
	return GetActorLocation();
}

FVector AMeleeWeapon::GetTraceStart() const
{
	if (CurrentTraceData && CurrentTraceData->Steps.IsValidIndex(CurrentTraceIndex))
	{
		return GetTrace(CurrentTraceData->Steps[CurrentTraceIndex].StartSocket);
	}
	else
		return GetActorLocation();
}

FVector AMeleeWeapon::GetTraceEnd() const
{
	if (CurrentTraceData && CurrentTraceData->Steps.IsValidIndex(CurrentTraceIndex))
		return GetTrace(CurrentTraceData->Steps[CurrentTraceIndex].EndSocket);
	else
		return GetActorLocation();
}

FName AMeleeWeapon::GetTraceStartName()
{
	if (CurrentTraceData && CurrentTraceData->Steps.IsValidIndex(CurrentTraceIndex))
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

