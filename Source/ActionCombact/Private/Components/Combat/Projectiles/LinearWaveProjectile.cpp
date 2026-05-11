// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Projectiles/LinearWaveProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ALinearWaveProjectile::ALinearWaveProjectile()
{
}

void ALinearWaveProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Dist = FVector::Dist(StartLocation, GetActorLocation());
	if (Dist >= MaxDist)
	{
		if (LinearWaveComp) LinearWaveComp->Deactivate();
		Destroy();
	}

	FVector Current = GetActorLocation();
	FVector Prev = LastLocation;

	TArray<FHitResult> Hits;
	FCollisionShape Shape = FCollisionShape::MakeSphere(80.f);

	GetWorld()->SweepMultiByChannel(Hits, Prev, Current, FQuat::Identity, ECC_Pawn, Shape);
	ProcessHitResults(Hits);
	LastLocation = Current;
}

void ALinearWaveProjectile::Init(const FProjectile& Config)
{
	Super::Init(Config);
	MaxDist = Config.Range;
}

void ALinearWaveProjectile::BeginPlay()
{
	LastLocation = GetActorLocation();
	StartLocation = GetActorLocation();
}

void ALinearWaveProjectile::ProcessHitResults(const TArray<FHitResult>& Hits)
{
	for (auto& Hit : Hits)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Hit.GetActor()))
		{
			CombatInterface->GetCombatComponent()->TryProcessTarget(Hit.GetActor(), Hit.ImpactPoint);
		}
	}
}

void ALinearWaveProjectile::SpawnLinearProjectile()
{
	if (!LinearWave) return;

	FVector SocketLocation = GetActorLocation();
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner())) CombatInterface->GetCombatMesh()->GetSocketLocation(FName("FX_SpecialAttacks"));
	LinearWaveComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		LinearWave,
		SocketLocation,
		FRotator::ZeroRotator,
		FVector(1.f)
	);

	if (!LinearWaveComp) return;
	LinearWaveComp->SetFloatParameter(TEXT("Lifetime"), 1.f);
}
