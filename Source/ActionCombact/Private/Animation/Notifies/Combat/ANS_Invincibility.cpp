// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/ANS_Invincibility.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void UANS_Invincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (IStatusReceiverInterface* Receiver = Cast<IStatusReceiverInterface>(MeshComp->GetOwner()))
	{
		Receiver->GetStatusComponent()->AddStatus(EStatusType::EST_Invincible);
	}
}

void UANS_Invincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (IStatusReceiverInterface* Receiver = Cast<IStatusReceiverInterface>(MeshComp->GetOwner()))
	{
		Receiver->GetStatusComponent()->RemoveStatus(EStatusType::EST_Invincible);
	}
}
