// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_FindItem_B.generated.h"

/**
 *
 */
UCLASS()
class BRAWLINN_API UBTS_FindItem_B : public UBTService
{
	GENERATED_BODY()

protected:

	UBTS_FindItem_B();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ItemToPickup;

};
