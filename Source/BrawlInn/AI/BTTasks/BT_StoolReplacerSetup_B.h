// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_StoolReplacerSetup_B.generated.h"

UCLASS()
class BRAWLINN_API UBT_StoolReplacerSetup_B : public UBTTaskNode
{
	GENERATED_BODY()
protected:

		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector Bar;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector IsDoorOpen;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector StoolSpawner;
	
};
