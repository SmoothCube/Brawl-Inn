// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_GetDropLocation_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "AI/AIItemManager_B.h"
#include "Items/Item_B.h"
#include "AI/AIDropPoint_B.h"

EBTNodeResult::Type UBT_GetDropLocation_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Manager = Cast<AAIItemManager_B>(UGameplayStatics::GetActorOfClass(GetWorld(), AAIItemManager_B::StaticClass()));
	if (!Manager)
	{
		BError("Can't find the AI Manager");
		return EBTNodeResult::Aborted;
	}

	if (Manager->ItemQueue.IsEmpty())
		return EBTNodeResult::Failed;

	AAIDropPoint_B* DropPoint = *Manager->ItemQueue.Peek();
	if (DropPoint)
	{
		Manager->ItemQueue.Pop();
		BScreen("Item");
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(DropActor.SelectedKeyName, DropPoint);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}