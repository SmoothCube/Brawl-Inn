// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "System/Structs/ScoreValues.h"
#include "ScoreSubSystem_B.generated.h"

DECLARE_EVENT_OneParam(UScoreSubSystem_B, FOnScoreChanged, FScoreValues);
DECLARE_EVENT(UScoreSubSystem_B, FOnScoreChangedNoParam);

UCLASS()
class BRAWLINN_API UScoreSubSystem_B : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	void AddScore(int Value, EScoreValueTypes Type = EScoreValueTypes::Score);

	void ResetScoreValues();

	UFUNCTION(BlueprintCallable)
	FScoreValues GetScoreValues() const;

	FOnScoreChanged OnScoreChanged;
	FOnScoreChangedNoParam OnScoreChangedNoParam;
private:

	FScoreValues ScoreValues;

};
