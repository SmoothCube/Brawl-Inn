// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTTasks/BT_BaseTask_B.h"
#include "BT_WalkTo_B.generated.h"

UCLASS()
class BRAWLINN_API UBT_WalkTo_B : public UBT_BaseTask_B
{
	GENERATED_BODY()

protected:
	UBT_WalkTo_B();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere)
		float AcceptanceRadius = 150.f;

	UPROPERTY()
		AActor* Target = nullptr;

};
