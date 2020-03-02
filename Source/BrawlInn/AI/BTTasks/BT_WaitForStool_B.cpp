// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_WaitForStool_B.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Items/Item_B.h"
#include "Characters/AI/AIController_B.h"

EBTNodeResult::Type UBT_WaitForStool_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = true;

	OwnerComponent = &OwnerComp;

	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Aborted;
	}

	return EBTNodeResult::InProgress;
}
void UBT_WaitForStool_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "AI", ActorsWithTag);
	if (ActorsWithTag.Num() > 0 && OwnerComponent)
	{
		OwnerComponent->GetBlackboardComponent()->SetValueAsObject(HoldingActor.SelectedKeyName, ActorsWithTag[0]);
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}

void UBT_WaitForStool_B::RecieveStool(AItem_B* Stool)
{
	if (OwnerComponent)
	{
		OwnerComponent->GetBlackboardComponent()->SetValueAsObject(HoldingActor.SelectedKeyName, Stool);
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}