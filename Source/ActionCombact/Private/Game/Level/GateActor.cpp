// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Level/GateActor.h"
#include "Game/ActionGameInstance.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Player/SlashPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "HUD/Interaction/InteractionWidget.h"

// Sets default values
AGateActor::AGateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	PortalEffect->SetupAttachment(GetRootComponent());
	SetRootComponent(PortalEffect);

	InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
	InteractCollision->SetupAttachment(GetRootComponent());

	InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	InteractionWidgetComponent->SetupAttachment(GetRootComponent());

	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawAtDesiredSize(true);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	InteractionWidgetComponent->SetPivot(FVector2D(0.f, 0.5f));
}

void AGateActor::BeginFocus()
{
	if (InteractionWidgetComponent)
	{
		ShouldUpdateWidgetPosition = true;
		InteractionWidgetComponent->SetVisibility(true);
		UpdateWidgetPosition();
	}
}

void AGateActor::EndFocus()
{
	if (InteractionWidgetComponent)
	{
		ShouldUpdateWidgetPosition = false;
		InteractionWidgetComponent->SetVisibility(false);
	}
}

void AGateActor::BeginInteract()
{
}

void AGateActor::EndInteract()
{
}

void AGateActor::Interact(AActor* Interactor)
{
	APawn* Player = Cast<APawn>(Interactor);
	if (!Player) return;
	UE_LOG(LogTemp, Warning, TEXT("Has Player"))
	ASlashPlayerController* PC = Cast<ASlashPlayerController>(Player->GetController());
	if (!PC) return;
	UE_LOG(LogTemp, Warning, TEXT("Has Controller"))
	EndFocus();
	PC->ShowGateConfirm(TargetLevelName, FSimpleDelegate::CreateUObject(this, &AGateActor::HandleGateConfirm));
}

const FInteractableData& AGateActor::GetInteractableData() const
{
	return InteractableData;
}

// Called when the game starts or when spawned
void AGateActor::BeginPlay()
{
	Super::BeginPlay();

	if (UInteractionWidget* Widget = Cast<UInteractionWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
	{
		Widget->UpdateWidget(InteractableData);
	}
}

void AGateActor::HandleGateConfirm()
{
	UActionGameInstance* GI = Cast<UActionGameInstance>(GetGameInstance());
	if (!GI) return;

	GI->TravelToLevel(TargetLevel);
}

void AGateActor::SetDestination(TSoftObjectPtr<UWorld> Target, FText Name)
{
	TargetLevel = Target;
	TargetLevelName = Name;
}

// Called every frame
void AGateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldUpdateWidgetPosition) UpdateWidgetPosition();
}

void AGateActor::UpdateWidgetPosition()
{
	if (!InteractionWidgetComponent || !InteractionWidgetComponent->IsVisible()) return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	if (!CameraManager) return;

	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector CameraUp = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	const FBoxSphereBounds Bounds = PortalEffect->Bounds;

	const FVector WidgetLocation = Bounds.Origin + CameraRight * (Bounds.SphereRadius + 20.f) + CameraUp * 20.f;

	InteractionWidgetComponent->SetWorldLocation(WidgetLocation);
}