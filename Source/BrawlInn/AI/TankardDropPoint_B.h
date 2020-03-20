// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIDropPoint_B.h"
#include "TankardDropPoint_B.generated.h"

UCLASS()
class BRAWLINN_API ATankardDropPoint_B : public AAIDropPoint_B
{
	GENERATED_BODY()

	void ItemDestroyed() override;
	
};
