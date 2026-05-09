// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "Items/Weapons/Data/HitEffectDataAsset.h"
#include "Items/Weapons/Data/CombatDataAsset.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Interfaces/HitInterface.h"
#include "Interfaces/StatusReceiverInterface.h"
#include "Interfaces/CombatInterface.h"

AWeapon::AWeapon()
{
}

void AWeapon::Equip(USceneComponent* InParent, const FName& InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetHitEffectData();

	DisableSphereCollision();
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

void AWeapon::DisableSphereCollision()
{
	if (OverlapSphere)
	{
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	InitializeSkills();
}

void AWeapon::InitializeSkills()
{
	Skills.Empty();

	for (int i = 0; i < WeaponData->SkillEntries.Num(); ++i)
	{
		FSkillEntry& Entry = WeaponData->SkillEntries[i];
		Entry.SkillSlot = static_cast<ESkillSlot>(i);

		USkillBase* RuntimeSkill = NewObject<USkillBase>(this, Entry.SkillClass);
		RuntimeSkill->Init(Entry);
		Skills.Add(RuntimeSkill);
	}
}

USkillBase* AWeapon::GetCurrentSkill(ESkillSlot Slot)
{
	if (Skills[int(Slot)])
	{
		return Skills[int(Slot)];
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
	if (Skills.IsValidIndex(0))
	{
		Skills[0]->ActivateSkill(GetOwner());
	}
}

void AWeapon::UseSkillE()
{
	if (Skills.IsValidIndex(1))
	{
		Skills[1]->ActivateSkill(GetOwner());
	}
}

void AWeapon::UseSkillR()
{
	if (Skills.IsValidIndex(2))
	{
		Skills[2]->ActivateSkill(GetOwner());
	}
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