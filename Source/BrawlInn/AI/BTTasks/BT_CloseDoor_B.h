// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_CloseDoor_B.generated.h"

class ABar_B;

UCLASS()
class BRAWLINN_API UBT_CloseDoor_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Bar;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector IsDoorOpen;

	UPROPERTY()
		UBehaviorTreeComponent* BehaviorTree = nullptr;

	UPROPERTY()
	ABar_B* Bar_B = nullptr;
};
