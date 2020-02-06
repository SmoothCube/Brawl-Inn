// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_CloseDoor_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

#include "Hazards/Bar_B.h"

EBTNodeResult::Type UBT_CloseDoor_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	BehaviorTree = &OwnerComp;

	Bar_B = Cast<ABar_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Bar.SelectedKeyName));
	Bar_B->OnDoorClosed_D.AddDynamic(this, &UBT_CloseDoor_B::DoorIsClosed);
	Bar_B->CloseDoor();
	return EBTNodeResult::InProgress;
}

void UBT_CloseDoor_B::DoorIsClosed()
{
	BehaviorTree->GetBlackboardComponent()->SetValueAsBool(IsDoorOpen.SelectedKeyName, false);
	FinishLatentTask(*BehaviorTree, EBTNodeResult::Succeeded);
	Bar_B->OnDoorClosed_D.RemoveDynamic(this, &UBT_CloseDoor_B::DoorIsClosed);
}
