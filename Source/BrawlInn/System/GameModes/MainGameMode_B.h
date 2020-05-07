// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"

#include "Containers/Queue.h"

#include "MainGameMode_B.generated.h"

class UBar_B;
class ATriggerBox;
class AGameCamera_B;
class UVictoryScreenWidget_B;
class USceneComponent;
class UGameOverlay_B;
class AGamePlayerController_B;
class ACameraActor;
class UAudioComponent;
class ALeaderFollower_B;

DECLARE_EVENT(AMainGameMode_B, FGameStart);
DECLARE_EVENT(AMainGameMode_B, FOnAnyScoreChange);

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	AMainGameMode_B();

	void PostLevelLoad() override;

	// ********** AActor **********

	void BeginPlay() override;

	void Tick(float DeltaTime) override;
	
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ********** Components **********
public:
	UBar_B* GetBar() const;

protected:
	UPROPERTY(VisibleAnywhere)
		UBar_B* BarComponent;

	// ********** PreGame **********

	void PreGame();

	void PregameCountdown();

	void PregameCountdownClock();

	void StartGame();

public:
	FGameStart& OnGameStart();

	UFUNCTION()
	void LastRespawnPawnDestroyed(AActor* DestroyedActor);

protected:
	FGameStart OnGameStart_Delegate;

	// ********** Game **********
	void UpdateClock();

	void CheckTimeVoicelines();

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
		int TimeRemainingBeforeMultipleScore = 20;

	void StartMultipleScore();

public:
	bool MultipleScoreIsActivated() const;
protected:
	bool bMultipleScore = false;

	// ********** PostGame **********
	void PostGame();

	bool bGameIsOver = false;

	// ********** Score	 **********
public:

	FOnAnyScoreChange& OnAnyScoreChange();

protected:

	FOnAnyScoreChange OnAnyScoreChange_Delegate;

	void SortPlayerControllersByScore(TArray<APlayerController_B*>& TempPlayerControllers);

	void CallOnAnyScoreChange() const;

	// ********** Camera **********
	UFUNCTION()
		void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		ATriggerBox* TrackingBox = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		TSubclassOf<ACameraActor> BP_FromCharacterSelectionCamera;

	UPROPERTY()
		ACameraActor* FromCharacterSelectionCamera = nullptr;

	// ********** UI **********

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidgets")
		TSubclassOf<UGameOverlay_B> BP_GameOverlay;
public:
	UPROPERTY()
		UGameOverlay_B* Overlay = nullptr;
private:

	UPROPERTY(EditAnywhere, Category = "UserWidgets")
	bool bShowGameOverlay = true;
#if WITH_EDITOR

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// ********** Sound **********
public:

	void ResetMusic();
protected:

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Countdown;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* StartSound;

	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* RoundOver;

	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* OneSecondRemaining;
	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* TwoSecondsRemaining;
	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* ThreeSecondsRemaining;
	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* FourSecondsRemaining;
	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* FiveSecondsRemaining;
	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* TenSecondsRemaining;

	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* OneMinuteRemaining;

	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* HalfwayPoint;

	UPROPERTY(EditAnywhere, Category = "Audio|Announcer")
		USoundCue* DoublePoints;
	

private:

	// ********** Timer **********

	FTimerHandle StartGameHandle;

	int PreGameCountdownTimer = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Clock")
		int GameTimeRemaining = 300;

	int FullGameTime = 0.f;

	FTimerHandle TH_CountdownTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Leader")
		int TimeBeforeMultiplyScoreAgainstLeader = 2;

	FTimerHandle TH_MultiplyScoresAgainstLeaderTimer;

	float Delay = 0.25;
	float CurrentTime = 0.0f;
	TQueue<FPlayerInfo> PlayerSpawnQueue;

	// ********** Leader **********
public:
	TArray<APlayerController_B*> GetLeadingPlayerController();

	void StartMultiplyingScores();

	bool ShouldUseScoreMultiplier() const;
protected:
	bool bMultiplyScoresAgainstLeader = false;

	UPROPERTY(EditDefaultsOnly, Category = "Leader")
		float AgainstLeaderMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Leader")
		TSubclassOf<ALeaderFollower_B> BP_LeaderFollower;

	UPROPERTY()
		ALeaderFollower_B* LeaderFollower = nullptr;
};

