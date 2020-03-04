// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_GetDropLocation_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "AI/AIDropPoint_B.h"

UBT_GetDropLocation_B::UBT_GetDropLocation_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_GetDropLocation_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);


	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
	if (!Bar)
	{
		BError("Can't find the Bar");
		return EBTNodeResult::Aborted;
	}

	return EBTNodeResult::InProgress;
}

void UBT_GetDropLocation_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (Bar && Bar->GetDropLocations(AICharacter)->PeekFront())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(DropLocationActor.SelectedKeyName, Bar->GetDropLocations(AICharacter)->PeekFront());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
