// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InteractItem.h"

AInteractItem::AInteractItem()
{
}

void AInteractItem::BeginFocus()
{
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AInteractItem::EndFocus()
{
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AInteractItem::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Interact"))
}

void AInteractItem::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("End Interact"))
}

void AInteractItem::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact"))
}
