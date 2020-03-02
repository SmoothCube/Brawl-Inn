// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_WaitForStool_B.generated.h"

class UBehaviorTreeComponent;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_WaitForStool_B : public UBTTaskNode
{
	GENERATED_BODY()

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	FName Tag = "AIStool";

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector HoldingActor;

	UPROPERTY()
		UBehaviorTreeComponent* OwnerComponent = nullptr;

	UPROPERTY()
	TArray<AActor*> ActorsWithTag;

};
