// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_WalkToSpawn_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"

UBT_WalkToSpawn_B::UBT_WalkToSpawn_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WalkToSpawn_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	bIsAtSpawn = OwnerComp.GetBlackboardComponent()->GetValueAsBool(IsAtSpawn.SelectedKeyName);

	return EBTNodeResult::InProgress;
}

void UBT_WalkToSpawn_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsAtSpawn)
	{
		const FVector TargetLocation = AICharacter->GetStartTransform().GetLocation();
		const FVector CharacterLocation = AICharacter->GetActorLocation();

		FVector Input = TargetLocation - CharacterLocation;
		Input.Z = 0;
		const float Distance = FVector::Dist(CharacterLocation, TargetLocation);
		if (Distance > AcceptanceRadius)
		{
			OwningAI->MoveToLocation(TargetLocation);
		}
		else {
			OwningAI->StopMovement();
			bIsAtSpawn = true;
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsAtSpawn.SelectedKeyName, bIsAtSpawn);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
