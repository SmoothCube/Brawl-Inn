// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "AIItemManager_B.generated.h"

class AAIDropPoint_B;

UCLASS()
class BRAWLINN_API AAIItemManager_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIItemManager_B();

	TQueue<AAIDropPoint_B*> ItemQueue;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
