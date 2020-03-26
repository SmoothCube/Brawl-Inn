// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "System/Structs/ScoreValues.h"
#include "ScoreSubSystem_B.generated.h"

DECLARE_EVENT_OneParam(UScoreSubSystem_B, FOnScoreValuesChanged, FScoreValues);
DECLARE_EVENT(UScoreSubSystem_B, OnScoreValuesChanged_NoParam);

UCLASS()
class BRAWLINN_API UScoreSubSystem_B : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	void AddScore(int Score);

	void ResetScoreValues();

	FScoreValues GetScoreValues() const;

	FOnScoreValuesChanged OnScoreValuesChanged;
	OnScoreValuesChanged_NoParam OnScoreValuesChanged_NoParam;
private:

	FScoreValues ScoreValues;

};
