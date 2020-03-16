// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_WalkToLocation_B.generated.h"

class AAIController_B;
class ACharacter_B;

UCLASS()
class BRAWLINN_API UBT_WalkToLocation_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UBT_WalkToLocation_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector TargetLocation;

	UPROPERTY(EditAnywhere)
		float AcceptanceRadius = 150.f;

	FVector Location;

	UPROPERTY()
		AAIController_B* OwningAI = nullptr;
	
	UPROPERTY()
	ACharacter_B* Character = nullptr;
};
