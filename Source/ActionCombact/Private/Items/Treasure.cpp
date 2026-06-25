// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInteface = Cast<IPickupInterface>(OtherActor);

	if (PickupInteface)
	{
		PickupInteface->AddGold(GoldAmount);
		Destroy();

		if (PickupEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
		}
	}
}

void ATreasure::SetGoldAmount(int32 Amount)
{
	GoldAmount = Amount;
}
