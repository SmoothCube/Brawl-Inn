// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BT_BaseTask_B.h"
#include "Hazards/Bar_B.h"
#include "BT_GetDropLocation_B.generated.h"

class UBar_B;
class AItem_B;

UCLASS()
class BRAWLINN_API UBT_GetDropLocation_B : public UBT_BaseTask_B
{
	GENERATED_BODY()

protected:

	UBT_GetDropLocation_B();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DropLocationActor;

	UPROPERTY(EditAnywhere)
		EBarDropLocationType Type = EBarDropLocationType::Stool;

	UPROPERTY()
		AItem_B* Item = nullptr;

	UPROPERTY()
		UBar_B* Bar = nullptr;
};
