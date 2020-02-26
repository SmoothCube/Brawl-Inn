// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleComponent.h"
#include "DestructibleComponent_B.generated.h"

/**
 *
 */
UCLASS()
class BRAWLINN_API UDestructibleComponent_B : public UDestructibleComponent
{
	GENERATED_BODY()

		virtual void ApplyDamage(float DamageAmount, const FVector& HitLocation, const FVector& ImpulseDir, float ImpulseStrength) override;

	bool bShouldFracture = false;
};
