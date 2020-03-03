// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_BaseTask_B.h"

#include "BrawlInn.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/AI/AICharacter_B.h"

EBTNodeResult::Type UBT_BaseTask_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComponent = &OwnerComp;
	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}

	AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find the AI Character");
		return EBTNodeResult::Aborted;
	}

	return Result;
}