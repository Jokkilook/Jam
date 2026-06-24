// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "EnemyBase.generated.h"

class UStatusComponent;

UCLASS()
class JAM_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExecuteAttack();

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void Die();

	UFUNCTION()
	void OnDeathFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackMontage;

	// 타격 시 잠깐 적용할 머티리얼 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	UMaterialInterface* HitMaterial;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	UStatusComponent* StatusComponent;

private:
	// 원본 머티리얼 저장용
	TArray<UMaterialInterface*> OriginalMaterials;

	FTimerHandle HitFlashTimerHandle;

	UFUNCTION()
	void RestoreOriginalMaterials();
};