// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Weapon/AN_HandleEquip.h"

#include "Interfaces/WeaponHolderInterface.h"

void UAN_HandleEquip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (IWeaponHolderInterface* WeaponHolder = Cast<IWeaponHolderInterface>(MeshComp->GetOwner())) WeaponHolder->HandleEquipState();
}