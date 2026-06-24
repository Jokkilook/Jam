// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomPatrol.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RandomPatrol::UBTTask_RandomPatrol()
{
	NodeName = TEXT("Random Patrol");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_RandomPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;
	
	APawn* Pawn = AIC->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;
	
	const FVector Origin = AIC->GetPawn()->GetActorLocation();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation RandomLocation;

	if (NavSys && NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(LocationKey.SelectedKeyName, RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
