// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Interfaces/LootReceiverInterface.h"

// Sets default values
AItem::AItem() : Amplitude(0.25f), TimeConstant(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetupAttachment(GetRootComponent());

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		const float rotationZ = DeltaTime * 100;

		ItemMesh->AddLocalRotation(FRotator(0.f, rotationZ, 0.f));
		ItemMesh->AddLocalOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

