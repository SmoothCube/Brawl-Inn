// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckIfFall_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "BrawlInn.h"
#include "AI/AIDropPoint_B.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"
#include "Items/Item_B.h"
#include "Components/HoldComponent_B.h"
#include "Hazards/Bar_B.h"
#include "System/GameModes/MainGameMode_B.h"

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

	if ((AICharacter->GetState() == EState::EBeingHeld && AICharacter->HoldComponent->IsHolding()) || (AICharacter->GetState() == EState::EFallen))
	{
		AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		check(IsValid(GameMode));
		UBar_B* Bar = GameMode->GetBar();
		if (Bar)
		{
			AAIDropPoint_B* DropPoint = Bar->GetDropLocations(AICharacter)->PeekFront();
			if (DropPoint && DropPoint == Cast<AAIDropPoint_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DropLocation.SelectedKeyName)))
			{
				DropPoint->SetNewItem(Cast<AItem_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(HoldingItem.SelectedKeyName)));
				Bar->GetDropLocations(AICharacter)->RemoveFront();
			}
		}
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->ClearValue(HoldingItem.SelectedKeyName);
		OwnerComp.GetBlackboardComponent()->ClearValue(DropLocation.SelectedKeyName);
		OwningAI->OnCharacterFall().Broadcast();
	}
}
