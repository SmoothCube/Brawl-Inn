// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControllerInterface_B.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllerInterface_B : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BRAWLINN_API IControllerInterface_B
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
		 void TakeOneDamage();
};
