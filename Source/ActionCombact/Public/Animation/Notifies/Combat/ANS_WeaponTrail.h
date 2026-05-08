// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_WeaponTrail.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 
 dk*/
UCLASS()
class ACTIONCOMBACT_API UANS_WeaponTrail : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = VisualEffects, Meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* TrailSystem;

	UPROPERTY(EditAnywhere, Category = VisualEffects, Meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TrailComp = nullptr;

	FVector StartLocation, EndLocation;
	float WeaponLength;
};
