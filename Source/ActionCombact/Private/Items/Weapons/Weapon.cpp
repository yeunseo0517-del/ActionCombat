// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/Status/StatusComponent.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "Items/Weapons/Data/HitEffectDataAsset.h"
#include "Items/Weapons/Data/CombatDataAsset.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Interfaces/HitInterface.h"
#include "Interfaces/StatusReceiverInterface.h"

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

	DisableSphereCollision();
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

void AWeapon::ClearPreviousData()
{
	ClearIgnoreArray();
	ClearPrevLocation();
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

void AWeapon::ClearIgnoreArray()
{
	IgnoreActors.Empty();
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

float AWeapon::CalculateDamage()
{
	float Damage = WeaponData->DefaultDamage;
	if (IStatusReceiverInterface* Receiver = Cast<IStatusReceiverInterface>(GetOwner()))
	{
		UStatusComponent* StatusComp = Receiver->GetStatusComponent();
		if (StatusComp)
		{
			Damage += StatusComp->GetEnhancedDamage();
		}
	}
	return Damage;
}

void AWeapon::ProcessHitResults(TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		IgnoreActors.AddUnique(Hit.GetActor());

		if (!WeaponData) return;
		
		float Damage = CalculateDamage();

		HandleHitResult(Hit, Damage);
	}
}

void AWeapon::HandleHitResult(const FHitResult& HitResult, float InDamage)
{
	if (HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			HitResult.GetActor(),
			InDamage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		ExecuteGetHit(HitResult);
		SpawnHitSparkParticles(HitResult);
	}
}

void AWeapon::ExecuteGetHit(const FHitResult& HitResult)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor()))
	{
		HitInterface->GetHit(HitResult.ImpactPoint, HitEffectData, GetOwner());
	}
}

void AWeapon::SpawnHitSparkParticles(const FHitResult& HitResult)
{
	if (HitEffectData && HitEffectData->HitSparkSystem)
	{
		HitEffectData->HitSpark = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HitEffectData->HitSparkSystem,
			HitResult.ImpactPoint
		);
	}
}
