// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Action/ANS_Dash.h"
#include "Interfaces/CombatInterface.h"
#include "Components/Combat/CombatComponent.h"

void UANS_Dash::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->GetCombatComponent()->ExecuteAction(FGameplayTags::Get().Action_Movement_Dash_Start);
}

void UANS_Dash::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner())) CombatInterface->GetCombatComponent()->ExecuteAction(FGameplayTags::Get().Action_Movement_Dash_End);
}
