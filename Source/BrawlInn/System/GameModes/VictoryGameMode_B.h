// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "VictoryGameMode_B.generated.h"

UCLASS()
class BRAWLINN_API AVictoryGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ********** AActor **********

	void BeginPlay() override;

	// ********** AGameMode_B **********
	
	void PostLevelLoad() override;

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly, Category = "Misc.")
	TSubclassOf<ACameraActor> IntroCamera_BP;

	UPROPERTY(EditDefaultsOnly, Category = "Misc.")
		TSubclassOf<ACameraActor> VictoryCamera_BP;
};
