// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Jam/JamCharacter.h"
#include "PlayerHUD.generated.h"

class UBuffSlot;
class UHorizontalBox;
/**
 * 
 */

UCLASS()
class JAM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
		
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

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ManaText;

	//텔레포트 슬롯
	UPROPERTY(meta = (BindWidget))
	class UImage* TeleportImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* TeleportCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* TeleportBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeleportCoolTimeText;
	
	//마나탄 슬롯
	UPROPERTY(meta = (BindWidget))
	UImage* ManaBulletImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBulletCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* ManaBulletBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ManaBulletCoolTimeText;
	
	//파이어볼 슬롯
	UPROPERTY(meta = (BindWidget))
	UImage* FireBallImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* FireBallCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* FireBallBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FireBallCoolTimeText;
	
	//아이스스톰 슬롯
	UPROPERTY(meta = (BindWidget))
	UImage* IceStormImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* IceStormCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* IceStormBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* IceStormCoolTimeText;
	
	//지진 슬롯
	UPROPERTY(meta = (BindWidget))
	UImage* EarthQuakeImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* EarthQuakeCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* EarthQuakeBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EarthQuakeCoolTimeText;
	
	//바인딩 슬롯
	UPROPERTY(meta = (BindWidget))
	UImage* BindingImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* BindingCoolTime;
	UPROPERTY(meta = (BindWidget))
	UImage* BindingBlockImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BindingCoolTimeText;

	//디버프 가로 박스
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* DebuffSlotList;

	//버프 가로 박스
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* BuffSlotList;

	//디버프 슬롯 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBuffSlot> BuffSlotClass;

	//버프 아이콘 리스트
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UTexture2D*> DebuffIconList;

	//버프 애님
	UPROPERTY(meta=(BindWidget))
	UImage* DebuffAlarm;
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* DebuffAlarmAnim;

	//부렵화음 스택
	UPROPERTY(meta=(BindWidget))
	UBuffSlot* DiscordSlot;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetHealthBar();
	UFUNCTION()
	void SetManaBar();
	UFUNCTION()
	void SetExpBar();
	UFUNCTION()
	void SetLevelText();

	UFUNCTION()
	void SetTeleportSlot();
	UFUNCTION()
	void SetManaBulletSlot();
	UFUNCTION()
	void SetFireBallSlot();
	UFUNCTION()
	void SetIceStormSlot();
	UFUNCTION()
	void SetEarthQuakeSlot();
	UFUNCTION()
	void SetBindingSlot();

	UFUNCTION()
	void OnDebuffActivated(int8 DebuffIndex);

	UFUNCTION(BlueprintCallable)
	void RefreshSlots();

	UFUNCTION(BlueprintCallable)
	void RefreshDebuffs();

	UFUNCTION(BlueprintCallable)
	void RefreshBuffs();

	UFUNCTION(BlueprintCallable)
	void FadeOut();

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;
};
