// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Characters/CharacterTypes.h"
#include "TeamInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTeamInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONCOMBACT_API ITeamInterface
{
	GENERATED_BODY()

public:
	virtual ETeamType GetTeamType() const = 0;
};
