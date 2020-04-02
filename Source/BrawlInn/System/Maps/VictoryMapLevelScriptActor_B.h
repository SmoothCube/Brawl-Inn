// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "System/GameInstance_B.h"
#include "VictoryMapLevelScriptActor_B.generated.h"

class AVictoryGameMode_B;

UCLASS()
class BRAWLINN_API AVictoryMapLevelScriptActor_B : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	AVictoryMapLevelScriptActor_B();

	// ********** AActor **********

	void BeginPlay() override;

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc.")
		FName GameMapName = "GameMap";

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		AVictoryGameMode_B* GameMode = nullptr;
	
};
