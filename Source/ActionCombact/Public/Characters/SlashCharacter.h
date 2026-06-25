#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AItem;
class ATreasure;
class AWeapon;
class UHealthBar;
class USkillWidget;

UCLASS()
class ACTIONCOMBACT_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HandleEquipState() override;
	virtual bool CanStartAttack() override;
	virtual void StartAttack() override;
	virtual void AttackEnd() override;

	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddGold(int32 Amount) override;

protected:
	virtual void BeginPlay() override;

	virtual void OnMontageEndedEvent(UAnimMontage* Montage, bool bInterrupted) override;
	virtual void EnterHitReact() override;

	/*
	Enchanced Input
*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* SlashCharacterContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BasicAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* QSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ESkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RSkillAction;

	UPROPERTY(EditAnywhere, Category = "Character Properties")
	float NormalWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Character Properties")
	float SprintWalkSpeed = 1000.f;

private:
	void InitializeSlashOverlay();
	void SetHUDHealth();
	void StartSprint();
	void StopSprint();
	void Equip();
	void UseQSkill();
	void UseESkill();
	void UseRSkill();
	void BasicAttack();


	/*
		Enhanced Input Action Callback Function
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump();
	void OnSprintStarted();
	void OnSprintStopped();
	void OnEquipStarted();
	void OnQStarted();
	void OnEStarted();
	void OnRStarted();

	void EquipWeapon(AWeapon* OverlappingWeapon);
	bool IsEquipMontage(UAnimMontage* Montage);
	bool CanArm();
	bool CanDisarm();
	void Arm();
	void Disarm();
	bool HasUnarmedWeapon();

	/*
	Play Montage
	*/
	void PlayEquipMontage(const FName& Section);

	/*
		State 
	*/
	void SetWeaponStance(EWeaponStance NewWeaponStance);

	/*
		Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY()
	class USlashOverlay* SlashOverlay;

};
