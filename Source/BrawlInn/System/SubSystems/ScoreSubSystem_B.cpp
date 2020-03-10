// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreSubSystem_B.h"

#include "BrawlInn.h"

void UScoreSubSystem_B::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UScoreSubSystem_B::Deinitialize()
{
}

void UScoreSubSystem_B::AddScore(int Score)
{
	ScoreValues.Score += Score;
	OnScoreValuesChanged.Broadcast(ScoreValues);
}

void UScoreSubSystem_B::ResetScoreValues()
{
	ScoreValues = FScoreValues();
}

FScoreValues UScoreSubSystem_B::GetScoreValues() const
{
	return ScoreValues;
}
