// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "ShopActor.generated.h"

UCLASS()
class ACTIONCOMBACT_API AShopActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AShopActor();
	virtual void Tick(float DeltaTime) override;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AActor* Interactor) override;

	virtual const FInteractableData& GetInteractableData() const override;

	TArray<struct FShopSlotData> GetShopItems();

protected:
	virtual void BeginPlay() override;

private:
	void UpdateWidgetPosition();
	void TryPurchase(const FName& ItemID);

	bool bInFocus = false;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ShopMesh;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<class UWidgetComponent> InteractionWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FInteractableData InteractableData;

	UPROPERTY(EditDefaultsOnly, Category = "Item Data")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	TArray<struct FShopEntry> ShopItems;
};
