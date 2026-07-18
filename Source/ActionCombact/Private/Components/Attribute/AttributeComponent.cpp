// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Attribute/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttributeComponent::Heal(float Value)
{
	if (Value <= 0) return;
	Health = FMath::Clamp(Health + Value, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UAttributeComponent::RecieveDamage(float DamageAmount)
{
	if (DamageAmount <= 0) return;
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

float UAttributeComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UAttributeComponent::GetCurrentHealth()
{
	return Health;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0;
}

