// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/SkillTypes.h"
#include "BaseProjectile.generated.h"

UCLASS()
class ACTIONCOMBACT_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void InitProjectile(const FProjectile& Config, FVector Dir);

protected:
	virtual void BeginPlay() override;
	void SetupCollision(UPrimitiveComponent* Collision);

	UPROPERTY(VisibleAnywhere)
	class UPrimitiveComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	class UNiagaraComponent* EffectComp;

	UPROPERTY()
	class UNiagaraSystem* Effect;

private:
	void FireInDirection(FVector Dir);
};
