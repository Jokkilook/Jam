// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSlot.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBuffSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBuffSlot::SetBuffSlot(UTexture2D* Icon, FText NewStackText, float CoolTimePercent)
{
	BuffImage->SetBrushFromTexture(Icon);
	StackText->SetText(NewStackText);
	CoolTime->SetPercent(CoolTimePercent);
}
