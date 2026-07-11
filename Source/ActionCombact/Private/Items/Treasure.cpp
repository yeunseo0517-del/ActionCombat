// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"

ATreasure::ATreasure()
{
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(ItemMesh);
}

void ATreasure::BeginPlay()
{
	Super::BeginPlay();

	// 이벤트가 발생했을 때 실행될 함수를 델리게이트에 바인딩
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ATreasure::OnSphereBeginOverlap);
}

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
