// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API AInteractItem : public AItem, public IInteractableInterface
{
	GENERATED_BODY()
	
public:
	AInteractItem();

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact() override;

protected:
};
