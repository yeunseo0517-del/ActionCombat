// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/AreaEffects/RadialShockwaves.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/WeaponHolderInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"
#include "Engine/OverlapResult.h"


ARadialShockwaves::ARadialShockwaves()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ARadialShockwaves::Tick(float DeltaTime)
{
	if (bExpandRadius)
	{
		SpawnShockwave(CurRadius);
		OnOverlap();

		CurRadius += Speed;
		if (CurRadius >= MaxRadius)
		{
			bExpandRadius = false;
			if (EffectComp) EffectComp->Deactivate();
			Destroy();
		}
	}
}

void ARadialShockwaves::InitShockwave(double InMaxRadius, float InDuration)
{
	MaxRadius = InMaxRadius;
	Duration = InDuration;
	ExpandImpactRadius();
}

void ARadialShockwaves::BeginPlay()
{
	Super::BeginPlay();
	
}

double ARadialShockwaves::CalculateRadiusFromOwner()
{
	if (!GetOwner()) return 0.f;
	IWeaponHolderInterface* Holder = Cast<IWeaponHolderInterface>(GetOwner());
	if (!Holder) return 0.f;
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Holder->GetTraceEnd();
	return FVector::Dist(Start, End);
}

void ARadialShockwaves::ExpandImpactRadius()
{
	CurRadius = CalculateRadiusFromOwner();
	Speed = FMath::Abs((MaxRadius - CurRadius) / Duration);
	bExpandRadius = true;
}

void ARadialShockwaves::SpawnShockwave(double Radius)
{
	if (!Effect) return;

	FVector RootLocation = GetOwner()->GetActorLocation();
	RootLocation.Z -= GetOwner()->GetRootComponent()->Bounds.BoxExtent.Z;
	EffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		Effect,
		RootLocation,
		FRotator::ZeroRotator,
		FVector(1.f)
	);

	if (!EffectComp) return;
	EffectComp->SetFloatParameter(TEXT("Lifetime"), 1.f);
	EffectComp->SetFloatParameter(TEXT("Radius"), Radius);
}

void ARadialShockwaves::OnOverlap()
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(CurRadius);
	FCollisionQueryParams Query;
	Query.AddIgnoredActor(GetOwner());
	Query.bTraceComplex = false;

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Query
	);

	ProcessHitResults(OverlapResults);
}

void ARadialShockwaves::ProcessHitResults(const TArray<FOverlapResult>& OverlapResults)
{
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
		{
			CombatInterface->GetCombatComponent()->TryProcessTarget(Overlap.GetActor(), Overlap.GetActor()->GetActorLocation());
		}
	}
}