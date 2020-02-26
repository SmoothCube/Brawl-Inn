// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleComponent_B.h"

void UDestructibleComponent_B::ApplyDamage(float DamageAmount, const FVector& HitLocation, const FVector& ImpulseDir, float ImpulseStrength)
{
	if (bShouldFracture)
	{
		Super::ApplyDamage(DamageAmount, HitLocation, ImpulseDir, ImpulseStrength);
	}
}
