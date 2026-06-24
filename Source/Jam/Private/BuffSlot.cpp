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
		float RemainingTime = 0.0f;
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
			case 5:
				RemainingTime = TM.GetTimerRemaining(JamCharacterRef->CoolZeroCoolTimer);
				CoolTimePercent = RemainingTime / JamCharacterRef->CoolZeroCoolTime;
				break;
			case 6:
				RemainingTime = TM.GetTimerRemaining(JamCharacterRef->GodCoolTimer);
				CoolTimePercent = RemainingTime / JamCharacterRef->GodCoolTime;
				break;
		}
		
		CoolTime->SetPercent(CoolTimePercent);

		if (FMath::IsNearlyEqual(RemainingTime, 0.0f) ||
			FMath::IsNearlyEqual(RemainingTime, -1.0f))
		{
			CoolTimeText->SetVisibility(ESlateVisibility::Collapsed);
		} else
		{
			CoolTimeText->SetVisibility(ESlateVisibility::Visible);
			FString RemainingStr = FString::Printf(TEXT("%.0f"), RemainingTime);
			CoolTimeText->SetText(FText::FromString(RemainingStr));
		}
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
