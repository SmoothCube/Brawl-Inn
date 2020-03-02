// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "System/Interfaces/ControllerInterface_B.h"
#include "AIController_B.generated.h"


UCLASS()
class BRAWLINN_API AAIController_B : public AAIController, public IControllerInterface_B
{
	GENERATED_BODY()

	AAIController_B(const FObjectInitializer& ObjectInitializer);
protected:

	// ********** Controller Interface **********
	virtual void TakeDamage_Implementation(int DamageAmount) override;

};
