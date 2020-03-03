// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "AICharacter_B.generated.h"

UCLASS()
class BRAWLINN_API AAICharacter_B : public ACharacter_B
{
	GENERATED_BODY()

		virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;


	FTransform StartTransform;
};
