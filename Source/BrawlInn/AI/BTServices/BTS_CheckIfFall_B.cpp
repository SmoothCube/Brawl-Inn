// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckIfFall_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"

UBTS_CheckIfFall_B::UBTS_CheckIfFall_B()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

void UBTS_CheckIfFall_B::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!OwningAI)
		OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());

	if (!OwningAI)
	{
		BError("Can't find AI controller");
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}
	if (!AICharacter)
		AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find AICharacter");
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}

	if (AICharacter->GetState() == EState::EFallen)
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->ClearValue(HoldingItem.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(HasFallen.SelectedKeyName, true);
		OwningAI->OnCharacterFall().Broadcast();
	}
	
}
