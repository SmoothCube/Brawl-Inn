// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameMode_B.h"
#include "MainGameMode_B.generated.h"

class AGameCamera_B;

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ** Overridden functions **

	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(APlayerController_B* PlayerController) override;

public:
	void PauseGame(APlayerController_B* ControllerThatPaused);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AGameCamera_B> BP_GameCamera;

private:
	AGameCamera_B* GameCamera = nullptr;

};
