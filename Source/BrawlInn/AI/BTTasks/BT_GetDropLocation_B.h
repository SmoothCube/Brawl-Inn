// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Hazards/Bar_B.h"
#include "BT_GetDropLocation_B.generated.h"

class AItem_B;

UCLASS()
class BRAWLINN_API UBT_GetDropLocation_B : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DropActor;

	UPROPERTY(EditAnywhere)
		EBarDropLocations Type = EBarDropLocations::Stool;

	UPROPERTY()
	AItem_B* Item = nullptr;

	UPROPERTY()
	ABar_B* Bar = nullptr;
};
