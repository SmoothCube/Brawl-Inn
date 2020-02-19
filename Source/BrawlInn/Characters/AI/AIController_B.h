// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "System/Interfaces/ControllerInterface_B.h"
#include "AIController_B.generated.h"

class AItem_B;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStoolReceived, AItem_B*);
DECLARE_MULTICAST_DELEGATE(FOnAIArrivedHome);

UCLASS()
class BRAWLINN_API AAIController_B : public AAIController, public IControllerInterface_B
{
	GENERATED_BODY()

public:
	AAIController_B(const FObjectInitializer& ObjectInitializer);

	// ********** Controller Interface **********
	virtual void TakeDamage_Implementation(int DamageAmount) override;

	// ********** BehaviorTree **********
	/// Called by the bar when it wants to place stools. Received by the BehaviourTree to the AI.
	FOnStoolReceived OnStoolReceived_D;

	/// Called when the AI is back at the bar and ready to place more stools.
	FOnAIArrivedHome OnAIArrivedHome_D;
};
