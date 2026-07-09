// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Level/GateActor.h"
#include "Game/ActionGameInstance.h"
#include "Components/BoxComponent.h"
#include "HUD/SlashHUD.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

// Sets default values
AGateActor::AGateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PortalEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalEffect"));
	SetRootComponent(PortalEffect);

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);

	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AGateActor::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AGateActor::OnGateBeginOverlap);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AGateActor::OnGateEndOverlap);
}

void AGateActor::OnGateBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	if (!OtherActor || OtherActor != PC->GetPawn()) return;

	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PC->GetHUD());
	if (!SlashHUD) return;
	
	SlashHUD->ShowGateConfirmWidget(TargetLevelName, FSimpleDelegate::CreateUObject(this, &AGateActor::HandleGateConfirm));
}

void AGateActor::OnGateEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	if (!OtherActor || OtherActor != PC->GetPawn()) return;

	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PC->GetHUD());
	if (SlashHUD) SlashHUD->HideGateConfirmWidget();
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
}
