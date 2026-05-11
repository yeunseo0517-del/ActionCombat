// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/AN_SpawnProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"

void UAN_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->GetCombatComponent()->OnAttackWindow();
}
