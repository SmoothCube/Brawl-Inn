// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AOEDamageAreaSubsystem_B.generated.h"

class AAOEDamageArea_B;

UCLASS()
class BRAWLINN_API UAOEDamageAreaSubsystem_B : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	TArray<AAOEDamageArea_B*> DamageAreas;

};
