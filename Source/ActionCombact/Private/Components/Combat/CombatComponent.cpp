// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/CombatComponent.h"
#include "Items/Weapons/Data/CombatDataAsset.h"
#include "Items/Weapons/Data/HitEffectDataAsset.h"

#include "Items/Weapons/Weapon.h"
#include "Characters/Base/BaseCharacter.h"
#include "Components/Combat/Skill/SkillBase.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/OverlapResult.h"

//#include "Interfaces/StatusReceiverInterface.h"
#include "Components/Status/StatusComponent.h"

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

	if (bExpandRadius)
	{
		SpawnShockwave(CurRadius);
		DoOverlap();

		CurRadius += Speed;
		if (CurRadius >= MaxRadius)
		{
			bExpandRadius = false;
			if(EffectComp) EffectComp->Deactivate();
		}
	}
}

bool UCombatComponent::CanAttack()
{
	return Character->CanStartAttack();
}

void UCombatComponent::ExecuteAttackSequence(FGameplayTag Tag)
{
	if (!CanAttack()) return;
	SetCurrentCombatTag(Tag);
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
	SetCurrentCombatTag(FGameplayTag());
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
	return CurrentCombatTag.IsValid();
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

void UCombatComponent::ExecuteAttack(const FGameplayTag& Tag)
{
	if (!Character) return;
	if (Tag.MatchesTag(FGameplayTags::Get().Action_Attack_Basic))
	{
		UpdateComboState();

		if (IsAttacking())
		{
			HandleCombo();
			return;
		}
	}

	ExecuteAttackSequence(Tag);
}

void UCombatComponent::ExecuteAction(const FGameplayTag& Tag)
{
	if (Tag.MatchesTag(FGameplayTags::Get().Action_Movement_Dash_Start))
	{
		StartDash();
	}
	else if (Tag.MatchesTag(FGameplayTags::Get().Action_Movement_Dash_End))
	{
		EndDash();
	}
}

void UCombatComponent::OnAttackWindow()
{
	if (!CurrentSkill) return;
	FGameplayTag Tag = CurrentSkill->GetTag();
	CurHitContext = BuildSkillHitContext();
	if (Tag == FGameplayTags::Get().Skill_Shockwave)
	{
		ExpandImpactRadius();
	}
}

void UCombatComponent::StartDash()
{
	Character->AddActionTag(FGameplayTags::Get().Action_Movement_Dash);
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
	Character->RemoveActionTag(FGameplayTags::Get().Action_Movement_Dash);
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
	CurrentState.ActionTag = CurrentCombatTag;
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

FCombatTraceData* UCombatComponent::GetCombatTraceData(const FGameplayTag& Tag) const
{
	if(OverrideCombatData)
		return OverrideCombatData->AttackSet.Find(Tag);
	return nullptr;
}

void UCombatComponent::SetCombatTraceData()
{
	if (EquippedWeapon)
		EquippedWeapon->SetCombatTraceData(GetCombatTraceData(CurrentCombatTag), CurrentCombatTag);
}

void UCombatComponent::SetCurrentCombatTag(const FGameplayTag& Tag)
{
	CurrentCombatTag = Tag;
}

float UCombatComponent::CalculateDamage(float DefaultDamage)
{
	float Damage = DefaultDamage;
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

void UCombatComponent::ExpandImpactRadius()
{
	CurRadius = CalculateRadiusFromOwner();
	if (CurrentSkill)
	{
		MaxRadius = CurrentSkill->GetMaxRadius();
		Speed = FMath::Abs((MaxRadius - CurRadius) / CurrentSkill->GetDuration());
		Shockwave = CurrentSkill->GetEffect();
	}
	bExpandRadius = true;
}

void UCombatComponent::SpawnShockwave(double Radius)
{
	if (!Shockwave) return;

	FVector RootLocation = GetOwner()->GetActorLocation();
	RootLocation.Z -= GetOwner()->GetRootComponent()->Bounds.BoxExtent.Z;
	EffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		Shockwave,
		RootLocation,
		FRotator::ZeroRotator,
		FVector(1.f)
	);

	if (!EffectComp) return;
	EffectComp->SetFloatParameter(TEXT("Lifetime"), 1.f);
	EffectComp->SetFloatParameter(TEXT("Radius"), Radius);
}

double UCombatComponent::CalculateRadiusFromOwner()
{
	if (!GetOwner()) return 0.f;
	IWeaponHolderInterface* Holder = Cast<IWeaponHolderInterface>(GetOwner());
	if (!Holder) return 0.f;
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Holder->GetTraceEnd();
	return FVector::Dist(Start, End);
}

void UCombatComponent::DoOverlap()
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(CurRadius);
	FCollisionQueryParams Query;
	Query.AddIgnoredActor(GetOwner());
	Query.bTraceComplex = false;
	
	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Query
	);
	
	ProcessOverlapResults(OverlapResults);
}

void UCombatComponent::ProcessHitResults(TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		TryProcessTarget(Hit.GetActor(), Hit.ImpactPoint);
	}
}

void UCombatComponent::ProcessOverlapResults(const TArray<FOverlapResult>& OverlapResults)
{
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		TryProcessTarget(Overlap.GetActor(), Overlap.GetActor()->GetActorLocation());
	}
}

void UCombatComponent::TryProcessTarget(AActor* Target, FVector ImpactPoint)
{
	if (!Target) return;
	if (CurHitContext.AlreadyHitActors.Contains(Target)) return;
	HandleHitResult(Target, ImpactPoint);
	CurHitContext.AlreadyHitActors.Add(Target);
}

void UCombatComponent::HandleHitResult(AActor* HitActor, FVector ImpactPoint)
{
	if (!HitActor) return;
	if (ProcessDamageApplication(HitActor))
	{
		ExecuteGetHit(HitActor, ImpactPoint);
		SpawnHitSparkParticles(ImpactPoint);
	}
}

bool UCombatComponent::ProcessDamageApplication(AActor* Target)
{
	if (!Target) return false;
	if (!IsHostile(Target)) return false;
	float Damage = CalculateDamage(CurHitContext.Damage);
	AController* InstigatorController = nullptr;
	if (APawn* Owner = Cast<APawn>(CurHitContext.Instigator))
		InstigatorController = Owner->GetController();
	UGameplayStatics::ApplyDamage(
		Target,
		Damage,
		InstigatorController,
		CurHitContext.DamageCauser,
		UDamageType::StaticClass()
	);
	return true;
}

void UCombatComponent::ExecuteGetHit(AActor* Hit, FVector ImpactPoint)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(Hit))
	{
		HitInterface->GetHit(ImpactPoint, CurHitEffectData, GetOwner());
	}
}


FHitContext UCombatComponent::BuildSkillHitContext()
{
	FHitContext HitContext;
	HitContext.Instigator = GetOwner();
	HitContext.DamageCauser = GetOwner();
	HitContext.AttackTag = CurrentSkill->GetTag();
	HitContext.Damage = CurrentSkill->GetSkillDamage();

	return HitContext;
}

bool UCombatComponent::IsHostile(AActor* OtherActor)
{
	if (ITeamInterface* Hitter = Cast<ITeamInterface>(CurHitContext.Instigator))
	{
		if (ITeamInterface* Other = Cast<ITeamInterface>(OtherActor))
		{
			FString Hit = UEnum::GetValueAsString(Hitter->GetTeamType());
			FString Ot = UEnum::GetValueAsString(Other->GetTeamType());
			UE_LOG(LogTemp,Warning,TEXT("Hit Actor: %s, Get Hit Actor: %s"), *Hit, *Ot)
			return Hitter->GetTeamType() != Other->GetTeamType();
		}
	}
	return false;
}

void UCombatComponent::SpawnHitSparkParticles(FVector ImpactPoint)
{
	if (CurHitEffectData && CurHitEffectData->HitSparkSystem)
	{
		CurHitEffectData->HitSpark = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			CurHitEffectData->HitSparkSystem,
			ImpactPoint
		);
	}
}