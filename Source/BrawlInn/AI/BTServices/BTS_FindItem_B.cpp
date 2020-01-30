// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_FindItem_B.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"
#include "Items/Item_B.h"

UBTS_FindItem_B::UBTS_FindItem_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

void UBTS_FindItem_B::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController_B* OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}
	AAICharacter_B* AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find AICharacter");
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}

	TArray<AActor*> OutActors;
	TArray<AItem_B*> Items;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem_B::StaticClass(), OutActors);
	if (OutActors.Num() == 0)
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}

	for (const auto& Actor : OutActors)
	{
		AItem_B* Item = Cast<AItem_B>(Actor);
		Items.Add(Item);
	}
	if (Items.Num() == 0)
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(ItemToPickup.SelectedKeyName);
		return;
	}

	Items.Sort([&](const AItem_B& LeftSide, const AItem_B& RightSide) {
		float DistanceA = FVector::Dist(AICharacter->GetActorLocation(), LeftSide.GetActorLocation());
		float DistanceB = FVector::Dist(AICharacter->GetActorLocation(), RightSide.GetActorLocation());
		return DistanceA < DistanceB;
		});

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(ItemToPickup.SelectedKeyName, Items[0]);
}
