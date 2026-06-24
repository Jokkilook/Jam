// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalDestination.h"
#include "StatusComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class JAM_API APortal : public AActor
{
	GENERATED_BODY()

public:
	APortal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	USphereComponent* TriggerVolume;

	// 레벨 에디터에서 인스턴스별로 설정
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal")
	APortalDestination* Destination;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Portal")
	uint8 RequiredLevel = 1;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UStatusComponent* PlayerStatus;

	bool bIsActive = false;

	UFUNCTION()
	void OnPlayerLevelChanged();

	UFUNCTION()
	void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void SetPortalActive(bool bActive);
};
