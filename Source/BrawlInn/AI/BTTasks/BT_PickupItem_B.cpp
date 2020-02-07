// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_PickupItem_B.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Items/Item_B.h"
#include "Characters/AI/AIController_B.h"
#include "Components/HoldComponent_B.h"
#include "Characters/AI/AICharacter_B.h"
#include "BrawlInn.h"

UBT_PickupItem_B::UBT_PickupItem_B()
{
}

EBTNodeResult::Type UBT_PickupItem_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	Item = Cast<AItem_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ItemToPickup.SelectedKeyName));

	if (Item)
	{
		if (AICharacter->HoldComponent->TryPickup())
		{
			OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(HoldingItem.SelectedKeyName, Item);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
