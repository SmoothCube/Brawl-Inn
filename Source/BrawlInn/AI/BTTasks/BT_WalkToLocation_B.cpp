// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_WalkToLocation_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#include "BrawlInn.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/Character_B.h"

UBT_WalkToLocation_B::UBT_WalkToLocation_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WalkToLocation_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Location = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocation.SelectedKeyName);
	BLog("G");

	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}
	
	Character = Cast<ACharacter_B>(OwningAI->GetCharacter());
	if (!IsValid(Character))
	{
		BError("Can't find the Character");
		return EBTNodeResult::Aborted;
	}
	return EBTNodeResult::InProgress;
}

void UBT_WalkToLocation_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	const FVector CharacterLocation = Character->GetActorLocation();

	FVector Input = Location - CharacterLocation;
	Input.Z = 0;
	const float Distance = FVector::Dist(CharacterLocation, Location);
	if (Distance > AcceptanceRadius)
	{
		OwningAI->MoveToLocation(Location);
	}
	else {
		OwningAI->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
