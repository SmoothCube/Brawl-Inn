// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_DropItem_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"
#include "Components/HoldComponent_B.h"
#include "Hazards/Bar_B.h"
#include "Items/Item_B.h"
#include "Items/Throwable_B.h"
#include "Items/Useable_B.h"
#include "AI/AIDropPoint_B.h"

UBT_DropItem_B::UBT_DropItem_B()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_DropItem_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwningAI = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!OwningAI)
	{
		BError("Can't find AIController");
		return EBTNodeResult::Aborted;
	}

	AICharacter = Cast<AAICharacter_B>(OwningAI->GetCharacter());
	if (!AICharacter)
	{
		BError("Can't find the AICharacter");
		return EBTNodeResult::Aborted;
	}

	AItem_B* Item = Cast<AItem_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(HoldingActor.SelectedKeyName));
	
	OwnerComp.GetBlackboardComponent()->ClearValue(HoldingActor.SelectedKeyName);

	
	AAIDropPoint_B* DropPoint =  Cast<AAIDropPoint_B>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(DropTargetPoint.SelectedKeyName));

	if (!DropPoint)
		return EBTNodeResult::Failed;

	AICharacter->HoldComponent->Drop();
	ABar_B* Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));

	Bar->GetDropLocations(AICharacter)->RemoveFront();

	AItem_B* NewItem = GetWorld()->SpawnActor<AItem_B>(Item->GetClass(), DropPoint->GetActorTransform());
	Item->Destroy();
	DropPoint->SetNewItem(NewItem);
	NewItem->SetActorLocationAndRotation(DropPoint->GetActorLocation(), DropPoint->GetActorRotation());

	OwnerComp.GetBlackboardComponent()->ClearValue(DropTargetPoint.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}
