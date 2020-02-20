// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_GetDropLocation_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "Hazards/Bar_B.h"
#include "Items/Item_B.h"
#include "AI/AIDropPoint_B.h"

EBTNodeResult::Type UBT_GetDropLocation_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
	if (!Bar)
	{
		BError("Can't find the Bar");
		return EBTNodeResult::Aborted;
	}

	if (Bar->GetStoolDropLocations().IsEmpty())
		return EBTNodeResult::Failed;

	AAIDropPoint_B* DropPoint = *Bar->GetStoolDropLocations().Peek();
	if (DropPoint)
	{
		Bar->GetStoolDropLocations().Pop();
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(DropActor.SelectedKeyName, DropPoint);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}