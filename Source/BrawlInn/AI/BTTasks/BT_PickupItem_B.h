// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_PickupItem_B.generated.h"

class AAIController_B;
class AAICharacter_B;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_PickupItem_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UBT_PickupItem_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ItemToPickup;

	AAIController_B* OwningAI = nullptr;
	AAICharacter_B* AICharacter = nullptr;
	AItem_B* Item = nullptr;
};
