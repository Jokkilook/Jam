// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSlot.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBuffSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBuffSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (JamCharacterRef)
	{
		FTimerManager& TM = GetWorld()->GetTimerManager();
		float CoolTimePercent = 0.0f;
		switch (BuffIndex)
		{
			case 2:
				CoolTimePercent = TM.GetTimerRemaining(JamCharacterRef->SpeedDebuffTimer) / JamCharacterRef->DebuffRemainingTime;
				break;
			case 3:
				CoolTimePercent = TM.GetTimerRemaining(JamCharacterRef->AttackDebuffTimer) / JamCharacterRef->DebuffRemainingTime;
				break;
			case 4:
				CoolTimePercent = TM.GetTimerRemaining(JamCharacterRef->DamageDebuffTimer) / JamCharacterRef->DebuffRemainingTime;
				break;
			
		}
		
		CoolTime->SetPercent(CoolTimePercent);
	}
}

void UBuffSlot::SetBuffSlot(UTexture2D* Icon, int8 NewBuffIndex, AJamCharacter* Owner)
{
	BuffImage->SetBrushFromTexture(Icon);
	StackText->SetText(FText::GetEmpty());
	BuffIndex = NewBuffIndex;

	if (Owner)
	{
		JamCharacterRef = Owner;
	}
	CoolTime->SetPercent(0.0f);
}
