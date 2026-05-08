// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/BaseCharacter.h"

#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/Data/HitEffectDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/Combat/Skill/SkillBase.h"
#include "Components/Attribute/AttributeComponent.h"
#include "Components/Status/StatusComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	ShieldComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Shield Component"));
	ShieldComp->SetupAttachment(GetMesh(), TEXT("HipsSocket"));
	ShieldComp->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ABaseCharacter::OnMontageEnded);
	}

	if (StatusComponent)
	{
		StatusComponent->OnStatusStart.AddDynamic(this, &ABaseCharacter::HandleStatusStart);
		StatusComponent->OnStatusEnd.AddDynamic(this, &ABaseCharacter::HandleStatusEnd);
		StatusComponent->OnSkillDeactivated.AddDynamic(this, &ABaseCharacter::HandleSkillDeactivated);
	}

	if (GetMesh()->DoesSocketExist(TEXT("HipsSocket")))
	{
		ShieldComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HipsSocket"));
	}

	if (ShieldComp && ShieldEffect)
	{
		ShieldComp->SetAsset(ShieldEffect);
	}

	SpawnDefaultWeapon();
	// 수정 필요
	WeaponStance = EquippedWeapon ? EWeaponStance::EWS_OneHand : EWeaponStance::EWS_Unarmed;
}

void ABaseCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), DefaultWeaponSocket, this, this);
		SwitchToWeapon(DefaultWeapon);
	}
}

void ABaseCharacter::SwitchToWeapon(AWeapon* NewWeapon)
{
	if (!NewWeapon || !Combat) return;
	NewWeapon->ApplySocketPolicy();
	Combat->OnWeaponEquipped(NewWeapon);
}

void ABaseCharacter::Die(const FName& Section)
{
	PlayDeathMontage(Section);
	BindDeathDelegate();
	StartCollisionTimer();
	SetLifeSpan(DeathLifeSpan);
}

void ABaseCharacter::BindDeathDelegate()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		FOnMontageBlendingOutStarted BlendingOutDelegate;
		BlendingOutDelegate.BindUObject(this, &ABaseCharacter::OnMontageBlendingOutStarted);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, DeathMontage);
	}
}

void ABaseCharacter::StartCollisionTimer()
{
	GetWorldTimerManager().SetTimer(CollisionTimer, this, &ABaseCharacter::DisableMeshCollision, 2.f);
}

void ABaseCharacter::OnMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage || !GetMesh() || Montage != DeathMontage) return;

	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
}

void ABaseCharacter::DisableMeshCollision()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void ABaseCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage) return;
	if (Combat && Combat->IsAttackMontage(Montage))
	{
		if (StatusComponent && StatusComponent->HasStatus(EStatusType::EST_SuperArmor))
		{
			StatusComponent->RemoveStatus(EStatusType::EST_SuperArmor);
		}
		Combat->AttackEnd(bInterrupted);
	}

	OnMontageEndedEvent(Montage, bInterrupted);
}

void ABaseCharacter::EnableTrace()
{
	if (Combat)
	{
		Combat->SetbTracing(true);
	}
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
	{
		ActiveWeapon->ClearPreviousData();
	}
}

void ABaseCharacter::DisableTrace()
{
	if (Combat)	Combat->SetbTracing(false);
}

FVector ABaseCharacter::GetTraceStart() const
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
		return ActiveWeapon->GetTraceStart();
	return GetActorLocation();
}

FVector ABaseCharacter::GetTraceEnd() const
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
		return ActiveWeapon->GetTraceEnd();
	return GetActorLocation();
}

USceneComponent* ABaseCharacter::GetWeaponMesh()
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
		return ActiveWeapon->GetWeaponMesh();
	return nullptr;
}

FName ABaseCharacter::GetTraceStartName()
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
		return ActiveWeapon->GetTraceStartName();
	return FName();
}

void ABaseCharacter::SwitchSccket(int32 Index)
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
	{
		ActiveWeapon->SetTraceIndex(Index);
	}
}

void ABaseCharacter::ResetSocket()
{
	if (AWeapon* ActiveWeapon = GetActiveWeapon())
	{
		ActiveWeapon->ResetTraceIndex();
	}
}

bool ABaseCharacter::IsAlive()
{
	return Attributes->IsAlive();
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(DamageCauser))
	{
		if (TeamInterface->GetTeamType() == TeamType) return 0.f;
	}
	if (IsInvincible()) return 0.f;
	Attributes->RecieveDamage(DamageAmount);
	return DamageAmount;
}

bool ABaseCharacter::IsInvincible()
{
	return StatusComponent && StatusComponent->HasStatus(EStatusType::EST_Invincible);
}

bool ABaseCharacter::IsSuperArmor()
{
	return StatusComponent && StatusComponent->HasStatus(EStatusType::EST_SuperArmor);
}

void ABaseCharacter::GetHit(const FVector& ImpactPoint, UHitEffectDataAsset* HitEffectData, AActor* Hitter)
{
	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(Hitter))
	{
		if (TeamInterface->GetTeamType() == TeamType) return;
	}
	if (IsInvincible() || IsSuperArmor()) return;
	StopMontage();

	double Theta = DirectionalHitReact(Hitter->GetActorLocation());
	FName Section = GetHitSection(Theta);
	if (IsAlive())
	{
		PlayHitReactMontage(Section);
	}
	else
	{
		Die(Section);
	}

	PlayHitSound(HitEffectData, ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::StopMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.1f); // 0.1초의 블렌드 아웃과 함께 중단
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			HitParticle,
			ImpactPoint
		);
	}
}

void ABaseCharacter::PlayHitSound(UHitEffectDataAsset* HitEffectData, const FVector& ImpactPoint)
{
	if (HitEffectData && HitEffectData->HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitEffectData->HitSound,
			ImpactPoint
		);
	}
}

bool ABaseCharacter::IsHitReactMontage(UAnimMontage* Montage)
{
	return Montage == HitReactMontage;
}

AWeapon* ABaseCharacter::GetActiveWeapon() const
{
	return EquippedWeapon ? EquippedWeapon : DefaultWeapon;
}

double ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	// 현재 캐릭터의 위치 vector
	const FVector Forward = GetActorForwardVector();

	// 맞은 지점의 vector 
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	double Theta = FMath::Acos(CosTheta);

	Theta = FMath::RadiansToDegrees(Theta);

	// -, + 방향 알아내기
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
		Theta *= -1.f;

	return Theta;
}

FName ABaseCharacter::GetHitSection(double Theta)
{
	FName Section = FName();

	if (Theta >= -45.f && Theta < 45.f)
		Section = FName("FromFront");
	else if (Theta >= 45.f && Theta < 135.f)
		Section = FName("FromRight");
	else if (Theta >= -135.f && Theta < -45.f)
		Section = FName("FromLeft");
	else
		Section = FName("FromBack");

	return Section;
}

void ABaseCharacter::HandleStatusStart(EStatusType Status)
{
	switch (Status)
	{
	case EStatusType::EST_EnhancedDamage:
		if (EquippedWeapon)
		{
			EquippedWeapon->PlayNiagaraEffect();
		}
		break;
	case EStatusType::EST_Invincible:
		ShieldComp->Activate(true);
		break;
	default:
		break;
	}
}

void ABaseCharacter::HandleStatusEnd(EStatusType Status)
{
	switch (Status)
	{
	case EStatusType::EST_EnhancedDamage:
		if (EquippedWeapon)
		{
			EquippedWeapon->DestroyNiagaraEffect();
		}
		break;
	case EStatusType::EST_Invincible:
		ShieldComp->DeactivateImmediate();
		break;
	default:
		break;
	}
}

void ABaseCharacter::HandleSkillDeactivated(ESkillSlot Slot)
{
	if (Combat)
	{
		Combat->SetCurrentSkill(nullptr);
	}
}

void ABaseCharacter::PlayMontage(UAnimMontage* Montage, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage);
		if (SectionName != NAME_None)
		{
			
			AnimInstance->Montage_JumpToSection(SectionName, Montage);
		}
	}
}

void ABaseCharacter::BasicAttack()
{
	if (Combat)
	{
		Combat->BasicAttack();
	}
}

void ABaseCharacter::PlayAttackMontage(UAnimMontage* Montage, FName SectionName)
{
	PlayMontage(Montage, SectionName);
}

void ABaseCharacter::CheckAndTriggerNextCombo()
{
	if (Combat) Combat->CheckAndTriggerNextCombo();
}

void ABaseCharacter::ResetCombo()
{
	if (Combat) Combat->ResetCombo();
}

USkeletalMeshComponent* ABaseCharacter::GetCombatMesh() const
{
	return GetMesh();
}

UCombatComponent* ABaseCharacter::GetCombatComponent() const
{
	if (Combat) return Combat; 
	return nullptr;
}

void ABaseCharacter::PlayHitReactMontage(const FName& Section)
{
	if(HitReactMontage)
		PlayMontage(HitReactMontage, Section);
}

void ABaseCharacter::PlayDeathMontage(const FName& Section)
{
	if (DeathMontage)
		PlayMontage(DeathMontage, Section);
}

