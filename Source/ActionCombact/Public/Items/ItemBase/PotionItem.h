// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase/ItemBase.h"
#include "PotionItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UPotionItem : public UItemBase
{
	GENERATED_BODY()
	
public:
	virtual void UseItem(AActor* TargetActor) override;
};
