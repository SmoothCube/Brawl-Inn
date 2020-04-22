// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
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

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerWin, AGamePlayerController_B*);
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


	// ********** Game **********
	void UpdateClock();

	// ********** PostGame **********

	bool bGameIsOver = false;

	// ********** Game States **********
	void StartGame();

	void EndGame();

public:

	FPlayerWin OnPlayerWin;
	FGameStart OnGameStart;

	// ********** Score	 **********
	void SortPlayerControllersByScore(TArray<APlayerController_B*>& TempPlayerControllers);

	FOnAnyScoreChange OnAnyScoreChange;

	void CallOnAnyScoreChange() const;

	// ********** Tracking **********
	UFUNCTION()
		void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	UPROPERTY()
		ATriggerBox* TrackingBox = nullptr;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		TSubclassOf<ACameraActor> BP_FromCharacterSelectionCamera;
	// ********** UI **********

	UPROPERTY()
		ACameraActor* FromCharacterSelectionCamera = nullptr;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidgets")
		TSubclassOf<UVictoryScreenWidget_B> BP_VictoryScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidgets")
		TSubclassOf<UGameOverlay_B> BP_GameOverlay;
private:
	UPROPERTY()
		UGameOverlay_B* Overlay = nullptr;

	// ********** Sound **********
public:

	void ResetMusic();
protected:

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Countdown;


private:

	// ********** Timer **********

	FTimerHandle StartGameHandle;

	int PreGameCountdownTimer = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Countdown")
		int GameTimeRemaining = 300;

	FTimerHandle TH_CountdownTimer;

	UPROPERTY(EditDefaultsOnly)
		int TimeBeforeMultiplyScoreAgainstLeader = 2;

	FTimerHandle TH_MultiplyScoresAgainstLeaderTimer;


	// ********** Leader **********
public:
	TArray<APlayerController_B*> GetLeadingPlayerController();

	void StartMultiplyingScores();

	bool ShouldUseScoreMultiplier() const;
protected:
	bool bMultiplyScoresAgainstLeader = false;

	UPROPERTY(EditDefaultsOnly)
		float AgainstLeaderMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ALeaderFollower_B> BP_LeaderFollower;

	UPROPERTY()
		ALeaderFollower_B* LeaderFollower = nullptr;
};

