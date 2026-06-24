// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalDestination.h"

// Sets default values
APortalDestination::APortalDestination()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortalDestination::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalDestination::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

