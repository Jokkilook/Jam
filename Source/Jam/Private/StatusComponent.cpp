// Fill out your copyright notice in the Description page of Project Settings.

#include "Jam/Public/StatusComponent.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStatusComponent::IncreaseHealth(float Amount)
{
	if (CurrentHealth >= MaxHealth) return;
	
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast();
}

void UStatusComponent::DecreaseHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);

	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
	}
	OnHealthChanged.Broadcast();
}	

void UStatusComponent::IncreaseMana(float Amount)
{
	if (CurrentMana >= MaxMana) return;
	
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);
	OnManaChanged.Broadcast();
}

void UStatusComponent::DecreaseMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana - Amount, 0.0f, MaxMana);

	if (CurrentMana <= 0)
	{
		CurrentMana = 0;
	}
	OnManaChanged.Broadcast();
}

void UStatusComponent::IncreaseExp(float Amount)
{
	CurrentExp += Amount;
	OnExpChanged.Broadcast();
}

void UStatusComponent::RefreshStatus()
{
	OnHealthChanged.Broadcast();
	OnManaChanged.Broadcast();
	OnExpChanged.Broadcast();
	OnLevelChanged.Broadcast();
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

