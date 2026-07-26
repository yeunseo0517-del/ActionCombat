// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "ShopActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchased, const int32)

struct FShopEntry;

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
	void TryPurchase(APawn* Buyer, const FName& ItemID, int32 Quantity);

	FOnItemPurchased OnItemPurchased;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateWidgetPosition();
	FShopEntry* FindItemByID(const FName& TargetID, int32& Index);

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
	TArray<FShopEntry> ShopItems;
};
