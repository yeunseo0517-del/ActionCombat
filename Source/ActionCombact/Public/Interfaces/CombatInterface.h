// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/CombatTypes.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *	Notify가 Character/Weapon 타입 없이
 *	전투 기능을 호출하기 위한 Interface
 */
class ACTIONCOMBACT_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EnableTrace() = 0;
	virtual void DisableTrace() = 0;
	virtual void CheckAndTriggerNextCombo() = 0;
	virtual void ResetCombo() = 0;

	virtual USkeletalMeshComponent* GetCombatMesh() const = 0;
	virtual class UCombatComponent* GetCombatComponent() const = 0;
};
