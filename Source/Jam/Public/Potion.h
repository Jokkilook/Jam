// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Potion.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class JAM_API APotion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 플레이어가 콜리전에 들어왔을 때 실행될 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 포션을 다시 리젠시키는 함수
	void RespawnItem();

public:    
	// 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	// 포션의 외형을 나타낼 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// 블루프린트에서 수정 가능한 체력 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float HealthRestoreAmount;

	// 블루프린트에서 수정 가능한 마나 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float ManaRestoreAmount;

	// 리젠 대기 시간 (기본값: 60초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float RespawnTime;

private:
	// 쿨타임을 관리할 타이머 핸들
	FTimerHandle RespawnTimerHandle;

};
