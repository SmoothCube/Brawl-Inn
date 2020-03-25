// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreSubSystem_B.h"

#include "BrawlInn.h"

void UScoreSubSystem_B::AddScore(const int Score)
{
	LastScoreValueAdded = Score;
	ScoreValues.Score += Score;
	OnScoreValuesChanged.Broadcast(ScoreValues);
	OnScoreValuesChanged_NoParam.Broadcast();
}

void UScoreSubSystem_B::ResetScoreValues()
{
	ScoreValues = FScoreValues();
}

FScoreValues UScoreSubSystem_B::GetScoreValues() const
{
	return ScoreValues;
}
