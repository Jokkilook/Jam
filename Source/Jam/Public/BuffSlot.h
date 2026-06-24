// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffSlot.generated.h"

/**
 * 
 */
UCLASS()
class JAM_API UBuffSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	class UImage* BuffImage;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* StackText;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* CoolTime;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetBuffSlot(UTexture2D* Icon, FText NewStackText, float CoolTimePercent);
};
