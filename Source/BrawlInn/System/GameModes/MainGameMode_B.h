// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "MainGameMode_B.generated.h"

class AGameCamera_B;
class UVictoryScreenWidget_B;
class USceneComponent;
class UPauseMenu_B;
class UGameOverlay_B;
class AGamePlayerController_B;
class ACameraActor;

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerWin, AGamePlayerController_B*);
DECLARE_EVENT(AMainGameMode_B, FGameOver);

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ** Overridden functions **

	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PreStartGame();

	void StartGame();

	void EndGame();

	virtual void Tick(float DeltaTime) override;

public:
	void PauseGame(AGamePlayerController_B* ControllerThatPaused);
	void ResumeGame();
	AGamePlayerController_B* PlayerControllerThatPaused = nullptr;

	FPlayerWin OnPlayerWin;
	FGameOver OnGameOver;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Camera")
		TSubclassOf<ACameraActor> BP_FromCharacterSelectionCamera;

	UPROPERTY()
		ACameraActor* FromCharacterSelectionCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UVictoryScreenWidget_B> BP_VictoryScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UGameOverlay_B> BP_GameOverlay;

	UPROPERTY(BlueprintReadWrite)
		UPauseMenu_B* PauseMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Birds;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* River;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Countdown;


private:

	FTimerHandle SwapCameraHandle;
	FTimerHandle StartGameHandle;
	int Timer = 3;

	FTimerHandle CountDownHandle;
	FTimerHandle StartGameHandle2;
	
	UPROPERTY()
		UGameOverlay_B* Overlay = nullptr;

	// ********** Timer **********

	UPROPERTY(EditDefaultsOnly)
		int TimeRemaining = 300;

	FTimerHandle TH_CountdownTimer;
};
