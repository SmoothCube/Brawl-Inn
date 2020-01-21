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

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AGameCamera_B> BP_GameCamera;


	AGameCamera_B* GameCamera = nullptr;

	UFUNCTION()
		void UpdateViewTarget(APlayerController_B* PlayerController);


};
