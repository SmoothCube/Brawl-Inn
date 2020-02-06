// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_StoolReplacerSetup_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

#include "Hazards/Bar_B.h"

EBTNodeResult::Type UBT_StoolReplacerSetup_B::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABar_B* Bar_B = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
	if (IsValid(Bar_B))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(Bar.SelectedKeyName, Bar_B);
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(StoolSpawner.SelectedKeyName, Bar_B->ItemSpawnLocation);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(IsDoorOpen.SelectedKeyName, false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	return EBTNodeResult::Aborted;
}