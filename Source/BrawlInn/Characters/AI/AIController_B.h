// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "System/Interfaces/ControllerInterface_B.h"
#include "AIController_B.generated.h"

class UHealthComponent_B;

UCLASS()
class BRAWLINN_API AAIController_B : public AAIController, public IControllerInterface_B
{
	GENERATED_BODY()

public:

	AAIController_B();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UHealthComponent_B* HealthComponent;

	virtual void TakeOneDamage_Implementation() override;
	
};
