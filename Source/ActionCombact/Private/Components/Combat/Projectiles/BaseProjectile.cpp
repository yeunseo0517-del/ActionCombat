// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Projectiles/BaseProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/CombatInterface.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CollisionComp);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bShouldBounce = false;
}

void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseProjectile::Init(const FProjectile& Config)
{
	ProjectileMovement->InitialSpeed = Config.Speed;
	ProjectileMovement->MaxSpeed = Config.Speed;
}

void ABaseProjectile::FireInDirection(FVector Dir)
{
	ProjectileMovement->Velocity = Dir * ProjectileMovement->InitialSpeed;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
}



