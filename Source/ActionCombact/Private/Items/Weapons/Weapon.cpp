// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/Combat/Skill/SkillBase.h"

#include "Data/HitEffectDataAsset.h"
#include "Data/CombatDataAsset.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Interfaces/HitInterface.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Interfaces/CombatInterface.h"

#include "Items/ItemBase/WeaponItem.h"

AWeapon::AWeapon()
{
}

void AWeapon::Equip(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetHitEffectData();
}

void AWeapon::InitializeFromItem(UWeaponItem* InItemBase)
{
	if (!InItemBase) return;
	ItemBase = InItemBase;

	const FItemData& Data = ItemBase->GetItemData();
	ItemMesh->SetStaticMesh(Data.ItemAssetData.Mesh);
	if (!Data.WeaponSetting.CombatData.IsNull()) WeaponData = Data.WeaponSetting.CombatData.LoadSynchronous();
	WeaponType = Data.WeaponSetting.WeaponType;

	InitializeSkills();
}

void AWeapon::DestroyWeapon()
{
	if (!ItemBase) return;
	ItemBase->RemoveBuff(GetOwner());
	Destroy();
}

void AWeapon::SetHitEffectData()
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		if (HitEffectData)
		{
			CombatInterface->GetCombatComponent()->SetHitEffectData(HitEffectData);
		}
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::InitializeSkills()
{
	Skills.Empty();
	int SkillCount = 0;
	if (!WeaponData || WeaponData->SkillLoadout.IsEmpty()) return;
	for (auto& It : WeaponData->SkillLoadout)
	{
		ESkillSlot Slot = It.Key;
		FSkillEntry& Entry = It.Value;

		USkillBase* RuntimeSkill = NewObject<USkillBase>(this, Entry.SkillClass);
		RuntimeSkill->Init(Entry, int32(Slot), SkillCount);
		Skills.Add(RuntimeSkill);
		SkillCount++;
	}
}

USkillBase* AWeapon::GetCurrentSkill(ESkillSlot Slot)
{
	if (!Skills.IsEmpty() && Skills[int32(Slot)])
	{
		return Skills[int32(Slot)];
	}
	return nullptr;
}

void AWeapon::ApplySocketPolicy()
{
	if (WeaponType == EWeaponType::EWT_Unarmed)
	{
		SetbUseCharacterSocket(true);
	}
	else
	{
		SetbUseCharacterSocket(false);
	}
}

void AWeapon::UseSkillQ()
{
	if (!Skills.IsEmpty() && Skills.IsValidIndex(0))
	{
		Skills[0]->ActivateSkill(GetOwner());
	}
}

void AWeapon::UseSkillE()
{
	if (!Skills.IsEmpty() && Skills.IsValidIndex(1))
	{
		Skills[1]->ActivateSkill(GetOwner());
	}
}

void AWeapon::UseSkillR()
{
	if (!Skills.IsEmpty() && Skills.IsValidIndex(2))
	{
		Skills[2]->ActivateSkill(GetOwner());
	}
}

FHitContext AWeapon::BuildWeaponHitContext(const TSet<AActor*>& AlreadyHit)
{
	FHitContext HitContext;

	HitContext.Instigator = GetInstigator() ? GetInstigator() : Cast<APawn>(GetOwner());
	HitContext.DamageCauser = this;
	HitContext.AttackTag = CurrentTraceData->AttackTag;

	return HitContext;
}

TArray<AActor*> AWeapon::BuildActorsToIgnore()
{
	TArray<AActor*> Actors;
	Actors.AddUnique(this);
	Actors.AddUnique(GetOwner());
	return Actors;
}

void AWeapon::PlayNiagaraEffect()
{
	if (WeaponData && WeaponData->EnhanceEffect)
	{
		BuffComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			WeaponData->EnhanceEffect,
			GetWeaponMesh(),
			GetTraceStartName(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
}

void AWeapon::DestroyNiagaraEffect()
{
	if (BuffComp) BuffComp->Deactivate();
}