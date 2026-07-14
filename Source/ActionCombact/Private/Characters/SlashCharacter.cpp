// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Types/Item/ItemAddResult.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Interfaces/InteractableInterface.h"

#include "Components/Attribute/AttributeComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/InventoryComponent.h"
#include "HUD/Battle/SkillHUDWidget.h"
#include "HUD/SlashHUD.h"
#include "HUD/Battle/SlashOverlay.h"

#include "Game/ActionGameInstance.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	TeamType = ETeamType::Player;
}

void ASlashCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime > InteractionCheckFrequency)) PerformInteractionCheck();
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
	UpdateHUDHealth();
	BindHUDInventory();
}

void ASlashCharacter::BindHUDInventory()
{
	if (ASlashHUD* HUD = GetSlashHUD())
		if (Inventory)
			HUD->BindInventory(Inventory);
}

void ASlashCharacter::UpdateHUDHealth()
{
	if(ASlashHUD* HUD = GetSlashHUD()) HUD->SetHealth(Attributes->GetCurrentHealth(), Attributes->GetMaxHealth());
}

ASlashHUD* ASlashCharacter::GetSlashHUD()
{
	if (SlashHUD.IsValid()) return SlashHUD.Get();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return nullptr;

	SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
	return SlashHUD.Get();
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
}

void ASlashCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	AActor* DetectedInteractable = nullptr;

	FVector TraceStart = GetPawnViewLocation() + 10.f;
	FVector TraceEnd = TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance);

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.f, 0, 2.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			AActor* HitActor = TraceHit.GetActor();
			if (HitActor && Cast<IInteractableInterface>(HitActor))
			{
				const float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				if (HitActor == InteractionData.CurrentInteractable.Get()) return;

				FoundInteractable(HitActor);
				return;
			}
		}
		NoInteractableFound();
	}
}

void ASlashCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable.IsValid())
	{
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractionData.CurrentInteractable.Get()))
			Interface->EndFocus();
	}

	IInteractableInterface* Interface = Cast<IInteractableInterface>(NewInteractable);
	if (!Interface) return;
	
	Interface->BeginFocus();
	
	if (ASlashHUD* HUD = GetSlashHUD())
	{
		if(InteractionData.CurrentInteractable.Get())
		HUD->ShowInteractionWidget(Interface->GetInteractableData());
	}

	InteractionData.CurrentInteractable = NewInteractable;
}

void ASlashCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(InteractionTimer);
	}

	if (InteractionData.CurrentInteractable.IsValid())
	{
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractionData.CurrentInteractable.Get()))
		{
			Interface->EndFocus();
		}
	}

	InteractionData.CurrentInteractable.Reset();
}

void ASlashCharacter::BeginInteract()
{
	// 시선이나 거리가 바뀌지 않았는지 확인
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable.IsValid())
	{
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractionData.CurrentInteractable.Get()))
		{
			Interface->BeginInteract();
			if (FMath::IsNearlyZero(Interface->GetInteractableData().InteractionDuration, .1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(InteractionTimer, this, &ASlashCharacter::Interact, Interface->GetInteractableData().InteractionDuration, false);
			}
		}
	}
}

void ASlashCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(InteractionTimer);

	if (InteractionData.CurrentInteractable.IsValid())
	{
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractionData.CurrentInteractable.Get()))
		{
			Interface->EndInteract();
		}
	}
}

void ASlashCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(InteractionTimer);

	if (InteractionData.CurrentInteractable.IsValid())
	{
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(InteractionData.CurrentInteractable.Get()))
		{
			Interface->Interact(this);
		}
	}
}

void ASlashCharacter::ToggleInventory()
{
	if (ASlashHUD* HUD = GetSlashHUD()) HUD->ToggleInventory();
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

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddGold(int32 Amount)
{
	if (Attributes)
	{
		UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
		if (!GI) return;
		GI->AddGoldData(Amount);
	}
}

FItemAddResult ASlashCharacter::AddItem(UItemBase* Item)
{
	return Inventory->HandleAddItem(Item);
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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASlashCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ASlashCharacter::ToggleInventory);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::OnEquipStarted);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &ASlashCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnQStarted);
		EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnEStarted);
		EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &ASlashCharacter::OnRStarted);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsInvincible()) return 0.f;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UpdateHUDHealth();
	return DamageAmount;
}