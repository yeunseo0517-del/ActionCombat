// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"

#include "Components/Attribute/AttributeComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "HUD/HealthBar.h"
#include "HUD/SkillHUDWidget.h"


// Sets default values
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(SpringArm);

	TeamType = ETeamType::Player;
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashCharacterContext, 0);
		}
	}
	CreateHealthBarWidget();
	CreateSkillWidget();
}

void ASlashCharacter::CreateHealthBarWidget()
{
	if (HealthBarClass && !HealthBar)
	{
		HealthBar = CreateWidget<UHealthBar>(GetWorld(), HealthBarClass);
		if (HealthBar) HealthBar->AddToViewport();
	}
}

void ASlashCharacter::CreateSkillWidget()
{
	if (SkillHUDClass && !SkillHUD)
	{
		SkillHUD = CreateWidget<USkillHUDWidget>(GetWorld(), SkillHUDClass);
		if (SkillHUD) SkillHUD->AddToViewport();
	}
}

void ASlashCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
}

void ASlashCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
}

void ASlashCharacter::BasicAttack()
{
	Attack(FGameplayTags::Get().Action_Attack_Basic);
}

void ASlashCharacter::OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsEquipMontage(Montage) || IsHitReactMontage(Montage))
		SetCurrentState(FGameplayTag());
}

void ASlashCharacter::EnterHitReact()
{
	if (IsDead()) return;
	Super::EnterHitReact();
	if (HealthBar)
	{
		HealthBar->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashCharacter::IsEquipMontage(UAnimMontage* Montage)
{
	return Montage == EquipMontage;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveValue = Value.Get<FVector2D>();

	if (GetController() && IsUnoccupied())
	{
		// Controller 의 앞 방향 찾기
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASlashCharacter::Jump()
{
	if (!IsUnoccupied()) return;
	Super::Jump();
}

void ASlashCharacter::OnSprintStarted()
{
	StartSprint();
}

void ASlashCharacter::OnSprintStopped()
{
	StopSprint();
}

void ASlashCharacter::OnEquipStarted()
{
	Equip();
}

void ASlashCharacter::Equip()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		// 무기 장착 해제
		if (CanDisarm())
		{
			SetCurrentState(FGameplayTags::Get().State_Player_Equip);
			PlayEquipMontage(FName("Unequip"));
		}
		// 무기 장착
		else if (CanArm())
		{
			SetCurrentState(FGameplayTags::Get().State_Player_Equip);
			PlayEquipMontage(FName("Equip"));
		}
	}
}

void ASlashCharacter::UseQSkill()
{
	if (EquippedWeapon && !HasUnarmedWeapon())
	{
		EquippedWeapon->UseSkillQ();
		if (Combat)
		{
			Combat->SetCurrentSkill(EquippedWeapon->GetCurrentSkill(ESkillSlot::Q));
		}
	}
}

void ASlashCharacter::UseESkill()
{
	if (EquippedWeapon && !HasUnarmedWeapon())
	{
		EquippedWeapon->UseSkillE();
		if (Combat)
		{
			Combat->SetCurrentSkill(EquippedWeapon->GetCurrentSkill(ESkillSlot::E));
		}
	}
}

void ASlashCharacter::UseRSkill()
{
	if (EquippedWeapon && !HasUnarmedWeapon())
	{
		EquippedWeapon->UseSkillR();
		if (Combat)
		{
			Combat->SetCurrentSkill(EquippedWeapon->GetCurrentSkill(ESkillSlot::R));
		}
	}
}

void ASlashCharacter::OnQStarted()
{
	UseQSkill();
}

void ASlashCharacter::OnEStarted()
{
	UseESkill();
}

void ASlashCharacter::OnRStarted()
{
	UseRSkill();
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	SetWeaponStance(EWeaponStance::EWS_OneHand);
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
	SwitchToWeapon(EquippedWeapon);
}

bool ASlashCharacter::CanArm()
{
	return EquippedWeapon && IsUnoccupied() && HasUnarmedWeapon();
}

bool ASlashCharacter::CanDisarm()
{
	return EquippedWeapon && IsUnoccupied() && !HasUnarmedWeapon();
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
		EquippedWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
		EquippedWeapon->Equip(GetMesh(), FName("SpineSocket"), this, this);
}

void ASlashCharacter::SetWeaponStance(EWeaponStance NewWeaponStance)
{
	WeaponStance = NewWeaponStance;
}

void ASlashCharacter::StartAttack()
{
	if (!CanStartAttack()) return;
	Super::StartAttack();
}


void ASlashCharacter::AttackEnd()
{
	if (!IsAttacking()) return;
	SetCurrentState(FGameplayTag());
}

void ASlashCharacter::HandleEquipState()
{
	if (!HasUnarmedWeapon())
	{
		Disarm();
		SetWeaponStance(EWeaponStance::EWS_Unarmed);
		SwitchToWeapon(DefaultWeapon);
	}
	else if (HasUnarmedWeapon())
	{
		Arm();
		SetWeaponStance(EWeaponStance::EWS_OneHand);
		SwitchToWeapon(EquippedWeapon);
	}
}

bool ASlashCharacter::CanStartAttack()
{
	return IsUnoccupied();
}

bool ASlashCharacter::HasUnarmedWeapon()
{
	return WeaponStance == EWeaponStance::EWS_Unarmed;
}

void ASlashCharacter::PlayEquipMontage(const FName& Section)
{
	if (EquipMontage)
		PlayMontage(EquipMontage, Section);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASlashCharacter::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASlashCharacter::OnSprintStopped);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::OnEquipStarted);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &ASlashCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnQStarted);
		EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnEStarted);
		EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnRStarted);
	}
}
