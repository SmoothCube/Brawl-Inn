// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_OpenDoor_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

#include "Hazards/Bar_B.h"

EBTNodeResult::Type UBT_OpenDoor_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	BehaviorTree = &OwnerComp;
	Bar_B = Cast<ABar_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Bar.SelectedKeyName));
	Bar_B->OnDoorOpen_D.AddDynamic(this, &UBT_OpenDoor_B::DoorIsOpened);
	Bar_B->OpenDoor();
	return EBTNodeResult::InProgress;
}

void UBT_OpenDoor_B::DoorIsOpened()
{
	BehaviorTree->GetBlackboardComponent()->SetValueAsBool(IsDoorOpen.SelectedKeyName, true);
	FinishLatentTask(*BehaviorTree, EBTNodeResult::Succeeded);
	Bar_B->OnDoorOpen_D.RemoveDynamic(this, &UBT_OpenDoor_B::DoorIsOpened);
}
