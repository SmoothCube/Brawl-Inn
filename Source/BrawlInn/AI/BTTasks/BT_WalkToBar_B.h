// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BT_BaseTask_B.h"
#include "BT_WalkToBar_B.generated.h"

UCLASS()
class BRAWLINN_API UBT_WalkToBar_B : public UBT_BaseTask_B
{
	GENERATED_BODY()

protected:

	UBT_WalkToBar_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector IsAtSpawn;

	UPROPERTY(EditAnywhere)
		float AcceptanceRadius = 150.f;

	bool bIsAtSpawn = false;

};
