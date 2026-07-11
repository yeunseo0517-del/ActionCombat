// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBACT_API AInteractItem : public AItem, public IInteractableInterface
{
	GENERATED_BODY()
	
public:
	AInteractItem();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AActor* Interactor) override;

	virtual const FInteractableData& GetInteractableData() const override;
	void UpdateInteractableData();

	FORCEINLINE class UItemBase* GetItemData() { return ItemInstance; }
protected:
	virtual void BeginPlay() override;

private:
	void InitializePickup(const int32 InQuantity);
	void InitializeDrop(const UItemBase* ItemToDrop, const int32 InQuantity);
	void UpdateWidgetPosition();

	bool ShouldUpdateWidgetPosition = false;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName DesiredItemID;

	UPROPERTY(EditInstanceOnly, Category = "Item Data")
	int32 ItemQauntity;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	UItemBase* ItemInstance;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	FInteractableData InteractableData;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<class UWidgetComponent> InteractionWidgetComponent;
};
