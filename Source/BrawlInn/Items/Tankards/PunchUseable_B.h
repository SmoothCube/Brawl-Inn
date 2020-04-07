// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Useable_B.h"
#include "PunchUseable_B.generated.h"

UCLASS()
class BRAWLINN_API APunchUseable_B : public AUseable_B
{
	GENERATED_BODY()

public:

	APunchUseable_B();

	virtual void Use_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "Boost")
		int StunStrength = 2;
};
