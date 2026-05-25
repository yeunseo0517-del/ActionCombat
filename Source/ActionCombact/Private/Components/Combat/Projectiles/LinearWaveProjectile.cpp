// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Projectiles/LinearWaveProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"

ALinearWaveProjectile::ALinearWaveProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetupCollision(SphereComp);
}

void ALinearWaveProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (LinearWaveComp)
	{
		LinearWaveComp->SetWorldLocation(GetActorLocation());
	}*/
	float Dist = FVector::Dist(StartLocation, GetActorLocation());
	if (Dist >= MaxDist)
	{
		if (LinearWaveComp) LinearWaveComp->Deactivate();
		Destroy();
	}

	FVector Current = GetActorLocation();
	FVector Prev = LastLocation;

	TArray<FHitResult> Hits;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	GetWorld()->SweepMultiByChannel(Hits, Prev, Current, FQuat::Identity, ECC_Pawn, Shape, Params);
	ProcessHitResults(Hits);
	LastLocation = Current;
}

void ALinearWaveProjectile::InitProjectile(const FProjectile& Config, FVector Dir)
{
	Super::InitProjectile(Config, Dir);
	MaxDist = Config.Range;

	SetupTransform();
	if (!LinearWave) return;
	SpawnProjectileNiagara();
}

void ALinearWaveProjectile::BeginPlay()
{
	Super::BeginPlay();
	LastLocation = GetActorLocation();
	StartLocation = GetActorLocation();
}

void ALinearWaveProjectile::ProcessHitResults(const TArray<FHitResult>& Hits)
{
	for (const auto& Hit : Hits)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
		{
			CombatInterface->GetCombatComponent()->TryProcessTarget(Hit.GetActor(), Hit.ImpactPoint);
		}
	}
}

void ALinearWaveProjectile::SetupTransform()
{
	FVector SocketLocation = GetActorLocation();
	FVector SocketRotation = FVector::ZeroVector;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		SocketLocation = CombatInterface->GetCombatMesh()->GetSocketLocation(StartSocketName);
		SocketRotation = CombatInterface->GetCombatMesh()->GetSocketRotation(StartSocketName).Vector();
	}
	SetActorLocation(SocketLocation);
	SetActorRotation(SocketRotation.Rotation());
}

void ALinearWaveProjectile::SpawnProjectileNiagara()
{
	LinearWaveComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		LinearWave,
		SphereComp,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
	);
}
