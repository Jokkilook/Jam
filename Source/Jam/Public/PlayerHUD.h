// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Jam/JamCharacter.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class JAM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	AJamCharacter* JamCharacterRef;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExpBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetHealthBar();
	UFUNCTION()
	void SetManaBar();
	UFUNCTION()
	void SetExpBar();
	UFUNCTION()
	void SetLevelText();
};
