// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/AN_ExecuteAreaAttack.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"
#include "Types/GameplayTags.h"

void UAN_ExecuteAreaAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->GetCombatComponent()->ExecuteAttack(FGameplayTags::Get().Skill_Shockwave);
}
