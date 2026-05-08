// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/Combat/ANS_WeaponTrail.h"

#include "Interfaces/WeaponHolderInterface.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"



void UANS_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (IWeaponHolderInterface* WeaponHolder = Cast< IWeaponHolderInterface>(MeshComp->GetOwner()))
	{
		TrailComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			WeaponHolder->GetWeaponMesh(),
			WeaponHolder->GetTraceStartName(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
}

void UANS_WeaponTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(TrailComp)
		TrailComp->DestroyComponent();
}

void UANS_WeaponTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!TrailComp) return;

	if (IWeaponHolderInterface* WeaponHolder = Cast< IWeaponHolderInterface>(MeshComp->GetOwner()))
	{
		StartLocation = WeaponHolder->GetTraceStart();
		EndLocation = WeaponHolder->GetTraceEnd();

		WeaponLength = FVector::Distance(StartLocation, EndLocation);
	}
	//FVector BladeDirection = (EndLocation - StartLocation).GetSafeNormal();

	// 위치 전달
	TrailComp->SetNiagaraVariableVec3(TEXT("StartPosition"), StartLocation);
	TrailComp->SetNiagaraVariableVec3(TEXT("EndPosition"), EndLocation);
	TrailComp->SetNiagaraVariableFloat(TEXT("WeaponLength"), WeaponLength);
	//TrailComp->SetNiagaraVariableVec3(TEXT("BladeDirection"), BladeDirection);
}
