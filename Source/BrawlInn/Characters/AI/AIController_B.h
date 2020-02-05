// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/PathFollowingComponent_B.h"

#include "System/Interfaces/ControllerInterface_B.h"
#include "AIController_B.generated.h"

class AItem_B;

DECLARE_MULTICAST_DELEGATE_OneParam(FStoolReceived, AItem_B*);
DECLARE_MULTICAST_DELEGATE(FOnAIArrivedHome);


UCLASS()
class BRAWLINN_API AAIController_B : public AAIController
{
	GENERATED_BODY()

public:

	AAIController_B(const FObjectInitializer& ObjectInitializer);

	 //virtual void TakeOneDamage_Implementation() override;

	/// Called by the bar when it wants to place stools. Received by the BehaviourTree to the AI.
	FStoolReceived OnStoolReceived_D;

	/// Called when the AI is back at the bar and ready to place more stools.
	FOnAIArrivedHome OnAIArrivedHome_D;
	
};
