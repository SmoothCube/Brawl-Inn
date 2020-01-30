// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_WalkTo_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Items/Item_B.h"
#include "Items/Throwable_B.h"
#include "System/DamageTypes/Stool_DamageType_B.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/AI/AICharacter_B.h"
#include "BrawlInn.h"

UBT_WalkTo_B::UBT_WalkTo_B()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBT_WalkTo_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}

	AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find the AI Character");
		return EBTNodeResult::Aborted;
	}

	Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActor.SelectedKeyName));

	return EBTNodeResult::InProgress;
}

void UBT_WalkTo_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (Target)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector CharacterLocation = AICharacter->GetActorLocation();

		FVector Input = TargetLocation - CharacterLocation;
		Input.Z = 0;
		float Distance = FVector::Dist(CharacterLocation, TargetLocation);
		if (Distance > AcceptanceRadius)
		{
			AICharacter->InputVector = Input.GetSafeNormal();
		}
		else {
			AICharacter->InputVector = FVector::ZeroVector;
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}