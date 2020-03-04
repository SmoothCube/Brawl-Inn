// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController_B.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterFall);

UCLASS()
class BRAWLINN_API AAIController_B : public AAIController
{
	GENERATED_BODY()

	AAIController_B(const FObjectInitializer& ObjectInitializer);
public:

	UPROPERTY(BlueprintAssignable)
	FOnCharacterFall OnCharacterFall;

};
