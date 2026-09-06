// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodFieldActor.h"
#include "BloodFieldSubSystem.h"
#include "Components/BoxComponent.h"

// Sets default values
ABloodFieldActor::ABloodFieldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FieldBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("FieldBounds"));
	RootComponent = FieldBounds;

	FieldBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FieldBounds->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void ABloodFieldActor::BeginPlay()
{
	Super::BeginPlay();
	
	UBloodFieldSubSystem* BloodFieldSubsystem = GetWorld()->GetSubsystem<UBloodFieldSubSystem>();
	if (!BloodFieldSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to Find Blood Field Subsystem"));
		return;
	}

	BloodFieldSubsystem->InitializeField(FVector3f(GetActorLocation()), FieldScale);
}

void ABloodFieldActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FieldBounds->SetBoxExtent(FVector(FieldScale) * 0.5);
}

// Called every frame
void ABloodFieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

