// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameMode_B.h"
#include "MainGameMode_B.generated.h"


class AGameCamera_B;
class UVictoryScreenWidget_B;
class UPauseMenu_B;


DECLARE_DELEGATE_OneParam(FPlayerWin, APlayerController_B*);

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ** Overridden functions **

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void UpdateViewTarget(APlayerController_B* PlayerController) override;

public:
	void PauseGame(APlayerController_B* ControllerThatPaused);
	void ResumeGame();
	APlayerController_B* PlayerControllerThatPaused = nullptr;

	FPlayerWin OnPlayerWin;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AGameCamera_B> BP_GameCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UVictoryScreenWidget_B> BP_VictoryScreen;

	UPROPERTY(BlueprintReadWrite)
	UPauseMenu_B* PauseMenuWidget = nullptr;


private:
	AGameCamera_B* GameCamera = nullptr;

};
