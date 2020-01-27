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
	virtual void Use_Implementation() override;
};
