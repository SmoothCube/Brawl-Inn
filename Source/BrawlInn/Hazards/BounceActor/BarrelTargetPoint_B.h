// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "BarrelTargetPoint_B.generated.h"

class UDecalComponent;
/**
 * 
 */
UCLASS()
class BRAWLINN_API ABarrelTargetPoint_B : public ATargetPoint
{
	GENERATED_BODY()
public:
	ABarrelTargetPoint_B();
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UDecalComponent* Decal;
};
