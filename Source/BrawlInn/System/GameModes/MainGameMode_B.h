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

	void StartGame();

public:
	FGameStart& OnGameStart();

protected:
	FGameStart OnGameStart_Delegate;
	
	// ********** Game **********
	void UpdateClock();

	// ********** PostGame **********
	void EndGame();

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

	UPROPERTY(EditDefaultsOnly, Category = "Leader")
		int TimeBeforeMultiplyScoreAgainstLeader = 2;

	FTimerHandle TH_MultiplyScoresAgainstLeaderTimer;

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

