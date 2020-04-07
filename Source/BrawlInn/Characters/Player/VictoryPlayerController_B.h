// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerController_B.h"
#include "VictoryPlayerController_B.generated.h"

class AVictoryGameMode_B;
UCLASS()
class BRAWLINN_API AVictoryPlayerController_B : public APlayerController_B
{
	GENERATED_BODY()

protected:
	// ********** AActor **********
	void BeginPlay() override;

	// ********** Buttons **********

	void FaceButtonBottomPressed() override;

	// ********** Misc. **********

	UPROPERTY(BlueprintReadOnly)
	AVictoryGameMode_B* VictoryGameMode = nullptr;
};
