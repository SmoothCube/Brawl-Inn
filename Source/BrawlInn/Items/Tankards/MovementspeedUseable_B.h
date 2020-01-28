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
	
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// ** Variables ** 
	UPROPERTY(EditDefaultsOnly, Category = "Variables|Boost")
	float MovementSpeedBoost = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Boost")
	float AccelerationBoost = 400;

private:
	FTimerHandle TH_DrinkHandle;
	class APlayerCharacter_B* Character;

	float OldMovementSpeed = 0;
	float OldAcceleration = 0;

	// ** Functions ** 
	UFUNCTION()
	void ResetBoost();
};
