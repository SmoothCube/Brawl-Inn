// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_WaitForStool_B.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "Items/Item_B.h"
#include "Characters/AI/AIController_B.h"

EBTNodeResult::Type UBT_WaitForStool_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComponent = &OwnerComp;

	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}

	OwningAI->OnStoolReceived_D.AddUObject(this, &UBT_WaitForStool_B::RecieveStool);

	return EBTNodeResult::InProgress;
}

void UBT_WaitForStool_B::RecieveStool(AItem_B* Stool)
{
	if (OwnerComponent)
	{
		OwnerComponent->GetBlackboardComponent()->SetValueAsObject(HoldingActor.SelectedKeyName, Stool);
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}