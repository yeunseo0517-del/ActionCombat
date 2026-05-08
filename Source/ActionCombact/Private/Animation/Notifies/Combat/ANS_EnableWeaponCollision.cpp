// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/ANS_EnableWeaponCollision.h"

#include "Interfaces/CombatInterface.h"

void UANS_EnableWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->EnableTrace();
}

void UANS_EnableWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->DisableTrace();
}
