// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_WalkToBar_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"
#include "BrawlInn.h"

UBT_WalkToBar_B::UBT_WalkToBar_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WalkToBar_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	BarSceneComponent = Cast<USceneComponent>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Bar.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBT_WalkToBar_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (BarSceneComponent)
	{
		FVector TargetLocation = AICharacter->StartTransform.GetLocation();
		FVector CharacterLocation = AICharacter->GetActorLocation();

		FVector Input = TargetLocation - CharacterLocation;
		Input.Z = 0;
		float Distance = FVector::Dist(CharacterLocation, TargetLocation);
		if (Distance > AcceptanceRadius)
		{
			OwningAI->MoveToLocation(TargetLocation);
		}
		else {
			OwningAI->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else {
		BError("Can't find target");
	}
}
