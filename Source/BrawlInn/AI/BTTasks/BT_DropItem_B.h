// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_DropItem_B.generated.h"

class AAIController_B;
class AAICharacter_B;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_DropItem_B : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DropTargetPoint;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector HoldingActor;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AItem_B> BP_Item;

	AAIController_B* OwningAI = nullptr;
	AAICharacter_B* AICharacter = nullptr;

};
