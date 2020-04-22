// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Useable_B.h"
#include "MovementspeedUseable_B.generated.h"

UCLASS()
class BRAWLINN_API AMovementspeedUseable_B : public AUseable_B
{
	GENERATED_BODY()

public:
	// ** Overridden functions **
	virtual void Use_Implementation() override;
	
protected:
	
	// ** Variables ** 
	UPROPERTY(EditDefaultsOnly, Category = "Boost")
	float MovementSpeedBoost = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Boost")
	float AccelerationBoost = 400;

private:
	class APlayerCharacter_B* Character;

	float OldMovementSpeed = 0;
	float OldAcceleration = 0;

	// ** Functions ** 
	virtual void ResetBoost() override;
};
