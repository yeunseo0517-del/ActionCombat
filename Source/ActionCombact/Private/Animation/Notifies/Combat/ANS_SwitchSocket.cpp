// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/ANS_SwitchSocket.h"
#include "Interfaces/WeaponHolderInterface.h"

void UANS_SwitchSocket::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (IWeaponHolderInterface* WeaponHolder = Cast<IWeaponHolderInterface>(MeshComp->GetOwner())) WeaponHolder->SwitchSccket(Index);
}

void UANS_SwitchSocket::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (IWeaponHolderInterface* WeaponHolder = Cast<IWeaponHolderInterface>(MeshComp->GetOwner())) WeaponHolder->ResetSocket();
}
