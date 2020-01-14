// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Base_DamageType_B.generated.h"

UCLASS()
class BRAWLINN_API UBase_DamageType_B : public UDamageType
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	int DamageMultiplier = 1;
};
