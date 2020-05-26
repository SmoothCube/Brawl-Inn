// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "VictoryGameMode_B.generated.h"

class UVictoryScreenWidget_B;
class ULevelSequencePlayer;
class ULevelSequence;

UCLASS()
class BRAWLINN_API AVictoryGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ********** AActor **********

	void BeginPlay() override;

	// ********** AGameMode_B **********

	void PostLevelLoad() override;

	void FadeToStatScreenDelayed();

	// ********** Level Sequence **********

	UPROPERTY(EditDefaultsOnly)
		ULevelSequence* FadeToBlackSequence;

	UPROPERTY()
		ULevelSequencePlayer* FadeToBlackSequencePlayer;

	UFUNCTION()
		void FadeToStatScreenFinished();

	// ********** Skip Widget **********

	UPROPERTY(EditDefaultsOnly, Category = "Skip")
		TSubclassOf<UUserWidget> Skip_BP;

	UPROPERTY()
		UUserWidget* Skip = nullptr;

public:
	void SetCanContinue(bool Value);
protected:

	UPROPERTY(BlueprintReadOnly)
		bool bCanContinue = false;

	UPROPERTY(EditDefaultsOnly, Category = "Skip")
		float ShowSkipTextDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skip")
		float StartFadeToScoreDelay = 6.f;

	UFUNCTION(BlueprintImplementableEvent)
		void HideSkipButton();

	// ********** Final Scores **********
public:
	UFUNCTION(BlueprintCallable)
		void FadeToStatScreen();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UVictoryScreenWidget_B> VictoryScreen_BP;

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly, Category = "Misc.")
		TSubclassOf<ACameraActor> VictoryCamera_BP;

	FTimerHandle FadeHandle;

	UFUNCTION(BlueprintImplementableEvent)
		void OpenBarDoor();

};
