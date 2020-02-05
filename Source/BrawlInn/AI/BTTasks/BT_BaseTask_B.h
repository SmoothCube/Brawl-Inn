// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_BaseTask_B.generated.h"

class AAIController_B;
class AAICharacter_B;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_BaseTask_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
		AAIController_B* OwningAI = nullptr;

	UPROPERTY()
		AAICharacter_B* AICharacter = nullptr;

	UPROPERTY()
		UBehaviorTreeComponent* OwnerComponent = nullptr;
	
};
