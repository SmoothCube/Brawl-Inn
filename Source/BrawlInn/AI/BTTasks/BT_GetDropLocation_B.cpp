// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_GetDropLocation_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
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

	if (Bar->GetDropLocations(Type).IsEmpty())
		return EBTNodeResult::Failed;

	AAIDropPoint_B* DropPoint = *Bar->GetDropLocations(Type).Peek();
	if (DropPoint)
	{
		Bar->GetDropLocations(Type).Pop();
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(DropActor.SelectedKeyName, DropPoint);
		Bar->StartTimerForNextStool();
		return EBTNodeResult::Succeeded;
	}
	else {
		BWarn("Cant find droppoint");
	}
	return EBTNodeResult::Failed;
}