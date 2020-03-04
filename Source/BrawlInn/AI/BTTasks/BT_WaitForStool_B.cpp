// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_WaitForStool_B.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/AI/AICharacter_B.h"
#include "Items/Item_B.h"

UBT_WaitForStool_B::UBT_WaitForStool_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WaitForStool_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComponent = &OwnerComp;

	return EBTNodeResult::InProgress;
}
void UBT_WaitForStool_B::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AItem_B* ItemToDeliver = AICharacter->GetItemDelivered();
	if (ItemToDeliver)
	{
		AItem_B* StoolToDeliver = GetWorld()->SpawnActor<AItem_B>(ItemToDeliver->GetClass(), AICharacter->GetActorLocation() + (FVector(150, 0, 0) * AICharacter->GetActorForwardVector()), FRotator());
		OwnerComponent->GetBlackboardComponent()->SetValueAsObject(ItemToPickup.SelectedKeyName, StoolToDeliver);
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}