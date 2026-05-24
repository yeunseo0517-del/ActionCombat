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
	if (LinearWaveComp)
	{
		LinearWaveComp->SetWorldLocation(GetActorLocation());
	}
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

	if (!LinearWave) return;
	SpawnLinearProjectile();
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

void ALinearWaveProjectile::SpawnLinearProjectile()
{
	FVector SocketLocation = GetActorLocation();
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner())) SocketLocation = CombatInterface->GetCombatMesh()->GetSocketLocation(FName("FX_SpecialAttacks"));
	else UE_LOG(LogTemp, Warning, TEXT("Fail to Cast"));
	LinearWaveComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		LinearWave,
		SocketLocation,
		FRotator::ZeroRotator,
		FVector(1.f)
	);
}
