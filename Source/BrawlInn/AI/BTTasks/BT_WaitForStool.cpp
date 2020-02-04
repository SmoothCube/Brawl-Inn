// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_WaitForStool.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "Items/Item_B.h"
#include "Characters/AI/AIController_B.h"

UBT_WaitForStool::UBT_WaitForStool()
{
}

EBTNodeResult::Type UBT_WaitForStool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComponent = &OwnerComp;

	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}

	OwningAI->OnStoolReceived_D.AddUObject(this, &UBT_WaitForStool::RecieveStool);

	return EBTNodeResult::InProgress;
}

void UBT_WaitForStool::RecieveStool(AItem_B* Stool)
{
	if (OwnerComponent)
	{
		OwnerComponent->GetBlackboardComponent()->SetValueAsObject(HoldingActor.SelectedKeyName, Stool);
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}