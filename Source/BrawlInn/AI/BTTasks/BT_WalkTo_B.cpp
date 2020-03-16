// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_WalkTo_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Characters/AI/AIController_B.h"
#include "Characters/AI/AICharacter_B.h"
#include "AI/PathFollowingComponent_B.h"
#include "BrawlInn.h"

UBT_WalkTo_B::UBT_WalkTo_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WalkTo_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActor.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBT_WalkTo_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (Target)
	{
		if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(bHasFallen.SelectedKeyName))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector CharacterLocation = AICharacter->GetActorLocation();

		FVector Input = TargetLocation - CharacterLocation;
		Input.Z = 0;
		const float Distance = FVector::Dist(CharacterLocation, TargetLocation);
		if (Distance > AcceptanceRadius)
		{
			OwningAI->MoveToActor(Target);
		}
		else {
			OwningAI->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else {
		BError("Can't find target");
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
}