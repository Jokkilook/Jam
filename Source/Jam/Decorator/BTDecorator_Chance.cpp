#include "BTDecorator_Chance.h"

UBTDecorator_Chance::UBTDecorator_Chance()
{
	NodeName = TEXT("Chance");
}

bool UBTDecorator_Chance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return FMath::FRand() <= Chance;
}

FString UBTDecorator_Chance::GetStaticDescription() const
{
	return FString::Printf(TEXT("%.0f%% 확률"), Chance * 100.f);
}
