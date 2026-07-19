// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase/WeaponItem.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Interfaces/EquipReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

void UWeaponItem::UseItem(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("In Use Item Function"))
	AddBuff(TargetActor);

	IEquipReceiverInterface* EquipReceiver = Cast<IEquipReceiverInterface>(TargetActor);
	EquipReceiver->EquipWeapon(this);
}

void UWeaponItem::AddBuff(AActor* TargetActor)
{
	IStatusReceiverInterface* StatusReceiver = Cast<IStatusReceiverInterface>(TargetActor);
	if (!StatusReceiver) return;

	UStatusComponent* StatusComp = StatusReceiver->GetStatusComponent();
	if (!StatusComp) return;

	StatusComp->AddEnhancedDamage(ItemData.ItemStatistics.DamageValue);
	StatusComp->AddDefense(ItemData.ItemStatistics.ArmorRating);
}

void UWeaponItem::RemoveBuff(AActor* Owner)
{
	IStatusReceiverInterface* StatusReceiver = Cast<IStatusReceiverInterface>(Owner);
	if (!StatusReceiver) return;

	UStatusComponent* StatusComp = StatusReceiver->GetStatusComponent();
	if (!StatusComp) return;

	StatusComp->RemoveEnhancedDamage(ItemData.ItemStatistics.DamageValue);
	StatusComp->RemoveDefense(ItemData.ItemStatistics.ArmorRating);
}
