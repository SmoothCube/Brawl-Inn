// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIDropPoint_B.h"
#include "ThrowableDropPoint_B.generated.h"

/**
 * 
 */
UCLASS()
class BRAWLINN_API AThrowableDropPoint_B : public AAIDropPoint_B
{
	GENERATED_BODY()

	void SpawnItem() override;
	
	void ItemDestroyed() override;
	
};
