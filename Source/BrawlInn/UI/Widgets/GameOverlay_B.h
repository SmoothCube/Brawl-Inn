// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay_B.generated.h"

class AGamePlayerController_B;
class AMainGameMode_B;
class UColoredTextBlock_B;
class UTextBlock;
class UVerticalBox;

UCLASS()
class BRAWLINN_API UGameOverlay_B : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void ShowScoreBoardAndClock();
	
	UFUNCTION(BlueprintImplementableEvent)
		void HideScoreBoardAndClock();
protected:
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* ScoreBox;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P1;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P2;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P3;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P4;

	UPROPERTY()
		AMainGameMode_B* GameMode = nullptr;

	// ********** Text **********
public:
	void DisplayText(FString FirstLine, FString SecondLine, float TimeToDisplay);
protected:

	UFUNCTION(BlueprintCallable)
		void OnFadeTextFinished(bool bIsPlayingForward);

	UFUNCTION(BlueprintImplementableEvent)
		void PlayFadeTextAnimation(bool bIsPlayingForward);

	UPROPERTY(meta = (BindWidget))
		UTextBlock* FirstTextLine;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* SecondTextLine;

	bool bShouldDisplay = false;

	float DisplayTime = 0.f;
	float CurrentDisplayTime = 0.f;

	// ********** Score **********

	void UpdateScoreList();

	UPROPERTY(BlueprintReadWrite)
		TArray<UColoredTextBlock_B*> ScoreArray;


	// ********** Timer **********
public:
	void UpdateTimerText(int TimeRemaining);

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TimeText;
};
