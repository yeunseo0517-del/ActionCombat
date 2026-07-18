// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase/PotionItem.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Attribute/AttributeComponent.h"

void UPotionItem::UseItem(AActor* TargetActor)
{
	IStatusReceiverInterface* StatusReceiver = Cast<IStatusReceiverInterface>(TargetActor);
	if (!StatusReceiver) return;

	UAttributeComponent* AttributeComp = StatusReceiver->GetAttributeComponent();
	if (!AttributeComp) return;

	AttributeComp->Heal(ItemData.ItemStatistics.RestorationAmount);
}