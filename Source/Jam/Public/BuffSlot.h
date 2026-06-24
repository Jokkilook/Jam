// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Jam/JamCharacter.h"
#include "BuffSlot.generated.h"

/**
 * 
 */
UCLASS()
class JAM_API UBuffSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AJamCharacter* JamCharacterRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 BuffIndex;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* BuffImage;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* StackText;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* CoolTime;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CoolTimeText;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetBuffSlot(UTexture2D* Icon, int8 NewBuffIndex, AJamCharacter* Owner);
};
