// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UIElementsInterface_B.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUIElementsInterface_B : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class BRAWLINN_API IUIElementsInterface_B
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
		void Tick();
	
public:
};
