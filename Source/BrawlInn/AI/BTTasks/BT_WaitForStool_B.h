// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BT_BaseTask_B.h"
#include "BT_WaitForStool_B.generated.h"

class AItem_B;

UCLASS()
class BRAWLINN_API UBT_WaitForStool_B : public UBT_BaseTask_B
{
	GENERATED_BODY()

		UBT_WaitForStool_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FName Tag = "AIStool";

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector ItemToPickup;

};
