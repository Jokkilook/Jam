// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManaChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExpChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JAM_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnHealthChanged OnHealthChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnManaChanged OnManaChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnExpChanged OnExpChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnLevelChanged OnLevelChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnDeath OnDeath;
	
public:

	UPROPERTY()
	float CurrentHealth = MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;
	UPROPERTY()
	float CurrentMana = MaxMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxMana = 100.0f;
	UPROPERTY()
	float CurrentExp = 0.0f;
	UPROPERTY()
	float MaxExp = 1000000.0f;
	UPROPERTY()
	uint8 Level = 1;
	
public:	
	// Sets default values for this component's properties
	UStatusComponent();

	//마나 회복 타이머
	FTimerHandle ManaRegenTimer;

	//스탯 반환 함수 - 값, 비율
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealthPercent() const { return CurrentHealth / MaxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentMana() const { return CurrentMana; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentManaPercent() const { return CurrentMana / MaxMana; }
	UFUNCTION(BlueprintCallable)
	uint8 GetLevel() const { return Level; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentExp() const { return CurrentExp; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentExpPercent() const { return CurrentExp / MaxExp; }
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetMaxMana() const { return MaxMana; }

	//스탯 영향 함수
	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(float Amount);
	UFUNCTION(BlueprintCallable)
	void DecreaseHealth(float Amount);
	UFUNCTION(BlueprintCallable)
	void IncreaseMana(float Amount);
	UFUNCTION(BlueprintCallable)
	void DecreaseMana(float Amount);
	UFUNCTION(BlueprintCallable)
	void IncreaseExp(float Amount);
	UFUNCTION(BlueprintCallable)
	void AddMaxHealth(float Amount) { MaxHealth += Amount; }
	UFUNCTION(BlueprintCallable)
	void AddMaxMana(float Amount) { MaxMana += Amount; }

	//상태 최신화
	UFUNCTION(BlueprintCallable)
	void RefreshStatus();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};