#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Chance.generated.h"

UCLASS()
class JAM_API UBTDecorator_Chance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_Chance();

	UPROPERTY(EditAnywhere, Category = "Chance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Chance = 0.5f;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
