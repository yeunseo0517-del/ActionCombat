// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/AN_ExecuteAreaAttack.h"
#include "Components/Combat/CombatComponent.h"
#include "Interfaces/CombatInterface.h"

void UAN_ExecuteAreaAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->GetCombatComponent()->OnAttackWindow();
}
