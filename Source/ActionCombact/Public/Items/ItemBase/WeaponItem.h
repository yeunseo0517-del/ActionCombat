// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase/ItemBase.h"
#include "WeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API UWeaponItem : public UItemBase
{
	GENERATED_BODY()
	
public:
	virtual void UseItem(AActor* TargetActor) override;
	void RemoveBuff(AActor* Owner);
	TSubclassOf<class AWeapon> GetWeaponClass() const { return ItemData.WeaponSetting.WeaponClass; }

private:
	void AddBuff(AActor* TargetActor);
};
