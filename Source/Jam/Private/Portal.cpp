// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetSphereRadius(100.f);
	RootComponent = TriggerVolume;
}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnTriggerOverlapBegin);

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			PlayerStatus = Pawn->FindComponentByClass<UStatusComponent>();
			if (PlayerStatus)
			{
				PlayerStatus->OnLevelChanged.AddDynamic(this, &APortal::OnPlayerLevelChanged);
				SetPortalActive(PlayerStatus->GetLevel() >= RequiredLevel);
			}
		}
	}
}

void APortal::OnPlayerLevelChanged()
{
	if (PlayerStatus)
		SetPortalActive(PlayerStatus->GetLevel() >= RequiredLevel);
}

void APortal::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive || !Destination) return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character) return;

	if (Cast<APlayerController>(Character->GetController()))
		Character->SetActorLocation(Destination->GetActorLocation());
}

void APortal::SetPortalActive(bool bActive)
{
	bIsActive = bActive;
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
}
