// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "Jam/EnemyBase.h"
#include "Animation/AnimInstance.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);
	if (!Enemy) return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;

	Enemy->ExecuteAttack();

	UBehaviorTreeComponent* OwnerCompPtr = &OwnerComp;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindLambda([this, OwnerCompPtr](UAnimMontage* EndedMontage, bool bInterrupted)
	{
		FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Succeeded);
	});
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Enemy->AttackMontage);

	return EBTNodeResult::InProgress;
}
