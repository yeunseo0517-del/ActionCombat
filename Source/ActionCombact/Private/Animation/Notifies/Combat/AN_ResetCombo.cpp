// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/AN_ResetCombo.h"

#include "Interfaces/CombatInterface.h"

void UAN_ResetCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner()))	CombatInterface->ResetCombo();
}