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

	// ********** BehaviorTree**********
	
public:
	FOnCharacterFall& OnCharacterFall();
protected:
	//Used to check if the character has fallen. Called from the BehaviorTree
	UPROPERTY(BlueprintAssignable)
	FOnCharacterFall OnCharacterFall_Delegate;

};
