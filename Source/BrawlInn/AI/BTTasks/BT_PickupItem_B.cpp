// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_PickupItem_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"

#include "Items/Item_B.h"
#include "Items/Throwable_B.h"
#include "System/DamageTypes/Stool_DamageType_B.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/AI/AICharacter_B.h"
#include "BrawlInn.h"

UBT_PickupItem_B::UBT_PickupItem_B()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBT_PickupItem_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AI controller");
		return EBTNodeResult::Failed;
	}
	AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find AICharacter");
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem_B::StaticClass(), OutActors);
	if (OutActors.Num() == 0)
		return EBTNodeResult::Failed;

	for (const auto& Actor : OutActors)
	{
		AItem_B* Item = Cast<AItem_B>(Actor);
			Items.Add(Item);
	}
	if (Items.Num() == 0)
		return EBTNodeResult::Failed;

	Items.Sort([&](const AItem_B& LeftSide, const AItem_B& RightSide) {
		float DistanceA = FVector::Dist(AICharacter->GetActorLocation(), LeftSide.GetActorLocation());
		float DistanceB = FVector::Dist(AICharacter->GetActorLocation(), RightSide.GetActorLocation());
		return DistanceA < DistanceB;
		});

	OwningAI->MoveToActor(Items[0], 10.f);

	return EBTNodeResult::Succeeded;
}

void UBT_PickupItem_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
