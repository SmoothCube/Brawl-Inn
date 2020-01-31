// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_WalkTo_B.generated.h"

class AAIController_B;
class AAICharacter_B;
class AItem_B;


UCLASS()
class BRAWLINN_API UBT_WalkTo_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UBT_WalkTo_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere)
		float AcceptanceRadius = 150.f;

	AAIController_B* OwningAI = nullptr;
	AAICharacter_B* AICharacter = nullptr;
	AActor* Target = nullptr;
};
