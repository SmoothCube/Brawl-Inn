// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckIfFall_B.generated.h"

class AAIController_B;
class AAICharacter_B;

UCLASS()
class BRAWLINN_API UBTS_CheckIfFall_B : public UBTService
{
	GENERATED_BODY()

protected:

	UBTS_CheckIfFall_B();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector ItemToPickup;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector HoldingItem;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector DropLocation;

	UPROPERTY()
		AAIController_B* OwningAI = nullptr;

	UPROPERTY()
		AAICharacter_B* AICharacter = nullptr;

};
