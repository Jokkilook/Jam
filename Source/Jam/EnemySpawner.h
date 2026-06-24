// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyBase;

UCLASS()
class JAM_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

public:
	// 스폰할 에너미 클래스 목록 (블루프린트에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<TSubclassOf<AEnemyBase>> EnemyClasses;

	// 최대 동시 유지 마릿수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "1"))
	int32 MaxEnemyCount = 3;

	// 스폰 반경 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "100.0"))
	float SpawnRadius = 500.f;

	// 죽은 뒤 재스폰까지 대기 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "0.0"))
	float RespawnDelay = 5.f;

	// false 로 끄면 재스폰하지 않음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bRespawn = true;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnEnemy();

private:
	TArray<TWeakObjectPtr<AEnemyBase>> SpawnedEnemies;

	void TryFillSpawnSlots();
	UFUNCTION()
	void OnEnemyDied(AActor* DestroyedActor);

	// 네비메시 위에서 유효한 랜덤 위치를 구함
	bool GetRandomSpawnLocation(FVector& OutLocation) const;
};
