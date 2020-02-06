// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_OpenDoor_B.generated.h"

/**
 *
 */
UCLASS()
class BRAWLINN_API UBT_OpenDoor_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Bar;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector IsDoorOpen;

	UFUNCTION()
		void DoorIsOpened();

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTree = nullptr;
};
