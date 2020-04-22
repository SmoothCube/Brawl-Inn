// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "MenuMapLevelScriptActor_B.generated.h"

class AMenuGameMode_B;
class UGameInstance_B;

UCLASS()
class BRAWLINN_API AMenuMapLevelScriptActor_B : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	AMenuMapLevelScriptActor_B();

	// ********** AActor **********

	void BeginPlay() override;

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc.")
		FName GameMapName = "GameMap";

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		AMenuGameMode_B* GameMode = nullptr;
};
