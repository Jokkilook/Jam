// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomPatrol.generated.h"

/**
 * 
 */
UCLASS()
class JAM_API UBTTask_RandomPatrol : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTTask_RandomPatrol();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LocationKey;
	
	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;
	
};
