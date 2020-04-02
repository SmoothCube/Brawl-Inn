// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "GameMapLevelScriptActor_B.generated.h"

class AMainGameMode_B;
class UGameInstance_B;

UCLASS()
class BRAWLINN_API AGameMapLevelScriptActor_B : public ALevelScriptActor
{
	GENERATED_BODY()

protected:

	AGameMapLevelScriptActor_B();

	// ********** AActor **********
	
	void BeginPlay() override;

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc.")
		FName GameMapName = "GameMap";

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Misc.")
		AMainGameMode_B* GameMode = nullptr;

};
