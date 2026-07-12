// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "GateActor.generated.h"

UCLASS()
class ACTIONCOMBACT_API AGateActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGateActor();

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AActor* Interactor) override;

	virtual const FInteractableData& GetInteractableData() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void UpdateWidgetPosition();

	bool ShouldUpdateWidgetPosition = false;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* InteractCollision;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<class UWidgetComponent> InteractionWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Gate")
	FInteractableData InteractableData;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* PortalEffect;

	UPROPERTY(EditAnywhere, Category = "Gate")
	TSoftObjectPtr<UWorld> TargetLevel;

	UPROPERTY(EditAnywhere, Category = "Gate")
	FText TargetLevelName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleGateConfirm();
	void SetDestination(TSoftObjectPtr<UWorld> Target, FText Name);
};
