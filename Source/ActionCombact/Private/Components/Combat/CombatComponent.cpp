// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/CombatComponent.h"
#include "Items/Weapons/Data/CombatDataAsset.h"

#include "Items/Weapons/Weapon.h"
#include "Characters/Base/BaseCharacter.h"
#include "Components/Combat/Skill/SkillBase.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ABaseCharacter>(GetOwner());
	UpdateComboState();
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTracing)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->DoTrace();
		}
	}

	if (bIsDashing)
	{
		FVector CurrentLoc = GetOwner()->GetActorLocation();
		FVector NewLoc = FMath::VInterpTo(CurrentLoc, DashTargetLoc, DeltaTime, 15.f);
		GetOwner()->SetActorLocation(NewLoc, true);
	}
}

bool UCombatComponent::CanAttack()
{
	return Character->CanStartAttack() && CurrentAttackType == EAttackType::EAT_None;
}

void UCombatComponent::BasicAttack()
{
	if (!Character) return;

	UpdateComboState();

	if (IsAttacking())
	{
		HandleCombo();
		return;
	}
	ExecuteAttackSequence(EAttackType::EAT_BasicAttack);
}

void UCombatComponent::ExecuteAttackSequence(EAttackType AttackType)
{
	if (!CanAttack()) return;
	SetAttackType(AttackType);
	SetCombatTraceData();
	Character->StartAttack();
	PlayAttackMontage();
}

void UCombatComponent::AttackEnd(bool bInterrupted)
{
	// 평타 콤보로 의도적으로 중간에 끊어 이은 부분은 적용되지 않도록
	if (bInterrupted && bComboTriggerd)
	{
		bComboTriggerd = false;
		return;
	}
	if (Character)
	{
		// 기본 상태로 초기화
		Character->AttackEnd();
	}
	SetAttackType(EAttackType::EAT_None);
	ResetCombo();
}

void UCombatComponent::HandleCombo()
{
	if (bUseCombo)
	{
		bNextCombo = true;
	}
}

bool UCombatComponent::IsAttacking()
{
	return CurrentAttackType != EAttackType::EAT_None;
}

void UCombatComponent::CheckAndTriggerNextCombo()
{
	if (bNextCombo)
	{
		bNextCombo = false;

		ComboCount++;

		if (GetCurrentAttackMontage())
		{
			bComboTriggerd = true;
			PlayAttackMontage();
			return;
		}
	}
}

void UCombatComponent::ResetCombo()
{
	bNextCombo = false;
	ComboCount = 0;
}

void UCombatComponent::PlayAttackMontage()
{
	if (!Character) return;

	FCombatState Key = GetCurrentCombatState();

	UAnimMontage* CurrentAttackMontage = GetCurrentAttackMontage();
	if (!CurrentAttackMontage) return;

	// Combo로 실행 시 설정해둔 시작지점 유무 체크
	// 해당 Section 존재 시 SectionIndex >= 0, 없으면 INDEX_NONE
	FName SectionName = GetComboSectionName(CurrentAttackMontage);
	Character->PlayAttackMontage(CurrentAttackMontage, SectionName);
}

FName UCombatComponent::GetComboSectionName(UAnimMontage* Montage)
{
	const FName ComboSection(TEXT("ComboEntry"));
	return (Montage->GetSectionIndex(ComboSection) != INDEX_NONE) ? ComboSection : NAME_None;
}

bool UCombatComponent::IsAttackMontage(UAnimMontage* Montage)
{
	if(FAnimMontageArray* MontageArray = GetMontageArray())
		return MontageArray->Montages.Contains(Montage);
	return false;
}

void UCombatComponent::OnWeaponEquipped(AWeapon* NewWeapon)
{
	EquippedWeapon = NewWeapon;
	ResetCombo();
	UpdateComboState();
}

void UCombatComponent::ExecuteSkill(FGameplayTag SkillTag)
{
	if (!Character) return;
	if (SkillTag.MatchesTag(FGameplayTags::Get().Skill_Area_Shockwave))
	{
		ExecuteAttackSequence(AttackType);
	}
}

void UCombatComponent::ExecuteAction(FGameplayTag ActionTag)
{
	UE_LOG(LogTemp,Warning,TEXT("Get in Execute Action"))
	
	UE_LOG(LogTemp, Warning, TEXT("Action Tag is %s"), *ActionTag.ToString())
	if (ActionTag.MatchesTag(FGameplayTags::Get().Action_Dash_Start))
	{
		
		StartDash();
	}
	else if (ActionTag.MatchesTag(FGameplayTags::Get().Action_Dash_End))
	{
		EndDash();
	}
}

void UCombatComponent::StartDash()
{
	if (CurrentSkill)
	{
		float Distance = CurrentSkill->GetDistance();
		DashTargetLoc = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * Distance);
		bIsDashing = true;
	}
}

void UCombatComponent::EndDash()
{
	bIsDashing = false;
}

FAnimMontageArray* UCombatComponent::GetMontageArray()
{
	FCombatState CurrentState = GetCurrentCombatState();
	return AnimMap.Find(CurrentState);;
}

void UCombatComponent::UpdateComboState()
{
	FAnimMontageArray* MontageArray = GetMontageArray();
	bUseCombo = MontageArray && MontageArray->Montages.Num() > 1;
}

FCombatState UCombatComponent::GetCurrentCombatState()
{
	FCombatState CurrentState;

	CurrentState.WeaponType = EquippedWeapon ? EquippedWeapon->GetWeaponType() : EWeaponType::EWT_Unarmed;

	CurrentState.AttackType = CurrentAttackType;

	CurrentState.WeaponStance = Character->GetWeaponStance();

	return CurrentState;
}

UAnimMontage* UCombatComponent::GetCurrentAttackMontage()
{
	FAnimMontageArray* MontageArray = GetMontageArray();
	if (MontageArray && MontageArray->Montages.IsValidIndex(ComboCount))
		return MontageArray->Montages[ComboCount];
	return nullptr;
}

FCombatTraceData* UCombatComponent::GetCombatTraceData(EAttackType AttackType) const
{
	if(OverrideCombatData)
		return OverrideCombatData->AttackSet.Find(AttackType);
	return nullptr;
}

void UCombatComponent::SetCombatTraceData()
{
	if (EquippedWeapon)
		EquippedWeapon->SetCombatTraceData(GetCombatTraceData(CurrentAttackType), CurrentAttackType);
}

void UCombatComponent::SetAttackType(EAttackType AttackType)
{
	CurrentAttackType = AttackType;
}
