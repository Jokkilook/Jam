// Fill out your copyright notice in the Description page of Project Settings.


#include "Jam/Public/PlayerHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Jam/Public/StatusComponent.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	JamCharacterRef = Cast<AJamCharacter>(GetOwningPlayerPawn());

	if (JamCharacterRef)
	{
		JamCharacterRef->StatusComponent->OnHealthChanged.AddDynamic(this, &UPlayerHUD::SetHealthBar);
		JamCharacterRef->StatusComponent->OnManaChanged.AddDynamic(this, &UPlayerHUD::SetManaBar);
		JamCharacterRef->StatusComponent->OnExpChanged.AddDynamic(this, &UPlayerHUD::SetExpBar);
		JamCharacterRef->StatusComponent->OnLevelChanged.AddDynamic(this, &UPlayerHUD::SetLevelText);
	}
}

void UPlayerHUD::SetHealthBar()
{
	if (JamCharacterRef)
	{
		HealthBar->SetPercent(JamCharacterRef->StatusComponent->GetCurrentHealthPercent());
	}
}

void UPlayerHUD::SetManaBar()
{
	if (JamCharacterRef)
	{
		ManaBar->SetPercent(JamCharacterRef->StatusComponent->GetCurrentManaPercent());
	}
}

void UPlayerHUD::SetExpBar()
{
	if (JamCharacterRef)
	{
		ExpBar->SetPercent(JamCharacterRef->StatusComponent->GetCurrentExp());
	}
}

void UPlayerHUD::SetLevelText()
{
	if (JamCharacterRef)
	{
		LevelText->SetText(FText::FromString(FString::FromInt(JamCharacterRef->StatusComponent->GetLevel())));
	}
}
