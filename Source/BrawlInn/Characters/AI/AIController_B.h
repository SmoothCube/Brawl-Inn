// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "System/Interfaces/ControllerInterface_B.h"
#include "AIController_B.generated.h"

class AItem_B;

DECLARE_MULTICAST_DELEGATE_OneParam(FStoolReceived, AItem_B*);

UCLASS()
class BRAWLINN_API AAIController_B : public AAIController, public IControllerInterface_B
{
	GENERATED_BODY()

public:

	AAIController_B();

	virtual void TakeDamage_Implementation(int DamageAmount) override;

	/// Called by the bar when it wants to place stools. Received by the BehaviourTree to the AI.
	FStoolReceived OnStoolReceived_D;
	
};
