// Fill out your copyright notice in the Description page of Project Settings.

#include "BT_WaitForStool_B.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "Hazards/Bar_B.h"
#include "BrawlInn.h"
#include "Items/Item_B.h"
#include "Characters/AI/AICharacter_B.h"

UBT_WaitForStool_B::UBT_WaitForStool_B()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBT_WaitForStool_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
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

void UBT_WaitForStool_B::Deliver(AItem_B* ItemToDeliver, AAICharacter_B* Character)
{
	//if (AICharacter == Character)
	//{
	//	AItem_B* StoolToDeliver = GetWorld()->SpawnActor<AItem_B>(ItemToDeliver->GetClass(), AICharacter->GetActorLocation() + FVector(50,0,0),FRotator());
	//	StoolToDeliver->Tags.Add("AIStool");
	//	
	//	BLog("DELIVER ITEM");
	//	OwnerComponent->GetBlackboardComponent()->SetValueAsObject(ItemToPickup.SelectedKeyName, StoolToDeliver);
	//	Bar->OnDeliverStart.RemoveAll(this);
	//	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	//}
}