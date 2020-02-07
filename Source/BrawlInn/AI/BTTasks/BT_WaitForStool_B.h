// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_WaitForStool_B.generated.h"

class AAIController_B;
class UBehaviorTreeComponent;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_WaitForStool_B : public UBTTaskNode
{
	GENERATED_BODY()

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
		void RecieveStool(AItem_B* Stool);

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector HoldingActor;

	UPROPERTY()
		AAIController_B* OwningAI = nullptr;

	UPROPERTY()
		UBehaviorTreeComponent* OwnerComponent = nullptr;;
};
