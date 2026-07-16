// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Battle/HealthBarComponent.h"
#include "HUD/Battle/HealthBar.h"
#include "Components/Attribute/AttributeComponent.h"

void UHealthBarComponent::BindAttribute(UAttributeComponent* Attribute) const
{
	if (HealthBarWidget) HealthBarWidget->BindAttribute(Attribute);
}