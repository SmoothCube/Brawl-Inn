// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_DropItem_B.generated.h"

class AAIController_B;
class AAICharacter_B;

UCLASS()
class BRAWLINN_API UBT_DropItem_B : public UBTTaskNode
{
	GENERATED_BODY()

		UBT_DropItem_B();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DropTargetPoint;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector HoldingActor;

	UPROPERTY()
	AAIController_B* OwningAI = nullptr;

	UPROPERTY()
	AAICharacter_B* AICharacter = nullptr;

};
