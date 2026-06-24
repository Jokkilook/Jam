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
<<<<<<< Updated upstream
=======
		JamCharacterRef->OnDebuffChanged.AddDynamic(this, &UPlayerHUD::RefreshDebuffs);
		JamCharacterRef->OnDebuffActivated.AddDynamic(this, &UPlayerHUD::OnDebuffActivated);
		JamCharacterRef->OnBuffChanged.AddDynamic(this, &UPlayerHUD::RefreshBuffs);
		RefreshBuffs();
		RefreshDebuffs();
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======

void UPlayerHUD::SetTeleportSlot()
{
	if (JamCharacterRef)
	{
		TeleportBlockImage->SetVisibility(JamCharacterRef->CanTeleport? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->TeleportCoolTimer))
		{
			TeleportCoolTime->SetVisibility(ESlateVisibility::Visible);
			TeleportCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->TeleportCoolTimer);
			const float CoolTime =  RemainingTime / JamCharacterRef->TeleportCoolTime;
			TeleportCoolTime->SetPercent(CoolTime);

			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			TeleportCoolTimeText->SetText(FText::FromString(RemainingStr));
		} else
		{
			TeleportCoolTime->SetVisibility(ESlateVisibility::Hidden);
			TeleportCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::SetManaBulletSlot()
{
	if (JamCharacterRef)
	{
		ManaBulletBlockImage->SetVisibility(JamCharacterRef->CanManaBullet ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->ManaBulletCoolTimer))
		{
			ManaBulletCoolTime->SetVisibility(ESlateVisibility::Visible);
			ManaBulletCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->ManaBulletCoolTimer);
			const float CoolTime =  RemainingTime / JamCharacterRef->ManaBulletCoolTime;
			ManaBulletCoolTime->SetPercent(CoolTime);

			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			ManaBulletCoolTimeText->SetText(FText::FromString(RemainingStr));
		} else
		{
			ManaBulletCoolTime->SetVisibility(ESlateVisibility::Hidden);
			ManaBulletCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::SetFireBallSlot()
{
	if (JamCharacterRef)
	{
		FireBallBlockImage->SetVisibility(JamCharacterRef->CanFireBall ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->FireBallCoolTimer))
		{
			FireBallCoolTime->SetVisibility(ESlateVisibility::Visible);
			FireBallCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->FireBallCoolTimer);
			const float CoolTime =  RemainingTime / JamCharacterRef->FireBallCoolTime;
			FireBallCoolTime->SetPercent(CoolTime);
			
			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			FireBallCoolTimeText->SetText(FText::FromString(RemainingStr));

		} else
		{
			FireBallCoolTime->SetVisibility(ESlateVisibility::Hidden);
			FireBallCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::SetIceStormSlot()
{
	if (JamCharacterRef)
	{
		IceStormBlockImage->SetVisibility(JamCharacterRef->CanIceStorm ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->IceStormCoolTimer))
		{
			IceStormCoolTime->SetVisibility(ESlateVisibility::Visible);
			IceStormCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->IceStormCoolTimer);
			const float CoolTime = RemainingTime / JamCharacterRef->IceStormCoolTime;
			IceStormCoolTime->SetPercent(CoolTime);

			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			IceStormCoolTimeText->SetText(FText::FromString(RemainingStr));

		} else
		{
			IceStormCoolTime->SetVisibility(ESlateVisibility::Hidden);
			IceStormCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::SetEarthQuakeSlot()
{
	if (JamCharacterRef)
	{
		EarthQuakeBlockImage->SetVisibility(JamCharacterRef->CanEarthQuake ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->EarthQuakeCoolTimer))
		{
			EarthQuakeCoolTime->SetVisibility(ESlateVisibility::Visible);
			EarthQuakeCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->EarthQuakeCoolTimer);
			const float CoolTime = RemainingTime / JamCharacterRef->EarthQuakeCoolTime;
			EarthQuakeCoolTime->SetPercent(CoolTime);

			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			EarthQuakeCoolTimeText->SetText(FText::FromString(RemainingStr));

		} else
		{
			EarthQuakeCoolTime->SetVisibility(ESlateVisibility::Hidden);
			EarthQuakeCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::SetBindingSlot()
{
	if (JamCharacterRef)
	{
		BindingBlockImage->SetVisibility(JamCharacterRef->CanBinding ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

		if (GetWorld()->GetTimerManager().IsTimerActive(JamCharacterRef->BindingCoolTimer))
		{
			BindingCoolTime->SetVisibility(ESlateVisibility::Visible);
			BindingCoolTimeText->SetVisibility(ESlateVisibility::Visible);
			const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(JamCharacterRef->BindingCoolTimer);
			const float CoolTime = RemainingTime / JamCharacterRef->BindingCoolTime;
			BindingCoolTime->SetPercent(CoolTime);

			FString RemainingStr = FString::Printf(TEXT("%.1f"), RemainingTime);
			BindingCoolTimeText->SetText(FText::FromString(RemainingStr));

		} else
		{
			BindingCoolTime->SetVisibility(ESlateVisibility::Hidden);
			BindingCoolTimeText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerHUD::OnDebuffActivated(int8 DebuffIndex)
{
	DebuffAlarm->SetBrushFromTexture(DebuffIconList[DebuffIndex]);
	PlayAnimation(DebuffAlarmAnim);
}

void UPlayerHUD::RefreshSlots()
{
	SetTeleportSlot();
	SetManaBulletSlot();
	SetFireBallSlot();
	SetIceStormSlot();
	SetEarthQuakeSlot();
	SetBindingSlot();
}

void UPlayerHUD::RefreshDebuffs()
{
	DebuffSlotList->ClearChildren();
	
	if (JamCharacterRef)
	{
		for (int8 Index : JamCharacterRef->DebuffList)
		{
			if (!BuffSlotClass) continue;
			
			UBuffSlot* NewSlot = CreateWidget<UBuffSlot>(GetWorld(), BuffSlotClass);
			NewSlot->SetBuffSlot(DebuffIconList[Index], Index, JamCharacterRef);
			NewSlot->AddToViewport();
			DebuffSlotList->AddChild(NewSlot);
		}
	}
}

void UPlayerHUD::RefreshBuffs()
{
	BuffSlotList->ClearChildren();
	
	if (JamCharacterRef)
	{
		FTimerManager& TM = GetWorld()->GetTimerManager();

		//쿨제로 버프 작동 중이면 아이콘 표시
		if (TM.IsTimerActive(JamCharacterRef->CoolZeroCoolTimer) ||
			TM.IsTimerActive(JamCharacterRef->CoolZeroRemainingTimer))
		{
			UBuffSlot* NewSlot = CreateWidget<UBuffSlot>(GetWorld(), BuffSlotClass);
			NewSlot->SetBuffSlot(DebuffIconList[5], 5, JamCharacterRef);
			BuffSlotList->AddChild(NewSlot);
		}

		//무적 버프 작동 중이면 아이콘 표시
		if (TM.IsTimerActive(JamCharacterRef->GodCoolTimer) ||
			TM.IsTimerActive(JamCharacterRef->GodRemainingTimer))
		{
			UBuffSlot* NewSlot = CreateWidget<UBuffSlot>(GetWorld(), BuffSlotClass);
			NewSlot->SetBuffSlot(DebuffIconList[6], 6, JamCharacterRef);
			BuffSlotList->AddChild(NewSlot);
		}
	}
}
>>>>>>> Stashed changes
