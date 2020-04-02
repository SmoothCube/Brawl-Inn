// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "MainGameMode_B.generated.h"

class ATriggerBox;
class AGameCamera_B;
class UVictoryScreenWidget_B;
class USceneComponent;
class UPauseMenu_B;
class UGameOverlay_B;
class AGamePlayerController_B;
class ACameraActor;
class UAudioComponent;
class ALeaderFollower_B;

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerWin, AGamePlayerController_B*);
DECLARE_EVENT(AMainGameMode_B, FGameStart);
DECLARE_EVENT(AMainGameMode_B, FOnAnyScoreChange);

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	AMainGameMode_B();

	// ********** Components **********
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UAudioComponent* MainMusicComponent;

	UFUNCTION(BlueprintCallable)
		void PostLevelLoad();

	// ** Overridden functions **


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;


	// ********** PreGame **********
	
	void PreGame();
	
	void PregameCountdown();

	void PregameCountdownClock();

	void UpdateClock();

	// ********** Game States **********
	void StartGame();

	void EndGame();

public:
	void PauseGame(AGamePlayerController_B* ControllerThatPaused);

	UFUNCTION(BlueprintCallable)
		void ResumeGame();

	FPlayerWin OnPlayerWin;
	FGameStart OnGameStart;

	// ********** Score	 **********

	FOnAnyScoreChange OnAnyScoreChange;

	void CallOnAnyScoreChange() const;

	// ********** Tracking **********
	UFUNCTION()
		void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	AGamePlayerController_B* PlayerControllerThatPaused = nullptr;

	UPROPERTY()
		ATriggerBox* TrackingBox = nullptr;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Camera")
		TSubclassOf<ACameraActor> BP_FromCharacterSelectionCamera;
	// ********** UI **********

	UPROPERTY()
		ACameraActor* FromCharacterSelectionCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UVictoryScreenWidget_B> BP_VictoryScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UGameOverlay_B> BP_GameOverlay;
private:
	UPROPERTY()
		UGameOverlay_B* Overlay = nullptr;

	// ********** Sound **********
public:
	void SetMusic(USoundCue* NewMusic);

	void ResetMusic();
protected:
	UPROPERTY(BlueprintReadWrite)
		UPauseMenu_B* PauseMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Birds;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* River;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Countdown;


private:

	// ********** Timer **********

	FTimerHandle StartGameHandle;

	int PreGameCountdownTimer = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Countdown")
		int GameTimeRemaining = 300;

	FTimerHandle TH_CountdownTimer;

	UPROPERTY(EditDefaultsOnly)
		int TimeBeforeMultiplyScoreAgainstLeader = 2;

	FTimerHandle TH_MultiplyScoresAgainstLeaderTimer;


	// ********** Leader **********
public:
	TArray<AGamePlayerController_B*> GetLeadingPlayerController();

	void StartMultiplyingScores();

	bool ShouldUseScoreMultiplier() const;
protected:
	bool bMultiplyScoresAgainstLeader = false;

	UPROPERTY(EditDefaultsOnly)
		float AgainstLeaderMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ALeaderFollower_B> BP_LeaderFollower;
};

