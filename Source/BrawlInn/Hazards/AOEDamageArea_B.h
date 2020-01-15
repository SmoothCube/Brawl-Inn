// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "AOEDamageArea_B.generated.h"

/**
 *
 */
UCLASS()
class BRAWLINN_API AAOEDamageArea_B : public ATriggerBox
{
	GENERATED_BODY()

public:
	AAOEDamageArea_B();
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Variables")
	TSubclassOf<UDamageType> AOEDamageType;

};
