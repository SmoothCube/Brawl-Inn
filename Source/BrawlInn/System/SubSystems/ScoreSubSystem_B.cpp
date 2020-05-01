// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreSubSystem_B.h"
#include "BrawlInn.h"

void UScoreSubSystem_B::AddPoints(const int Value, const bool bIsMultiplied, const bool bIsAgainstLeader)
{
	ScoreValues.Score += Value;
	if (ScoreValues.Score < 0)
		ScoreValues.Score = 0;

	ScoreValues.bIsMultiplied = bIsMultiplied;
	ScoreValues.bIsAgainstLeader = bIsAgainstLeader;
	
	ScoreValues.LastScoreAdded = Value;
	OnScoreChanged.Broadcast(ScoreValues);
	OnScoreChangedNoParam.Broadcast();
}

void UScoreSubSystem_B::AddStats(const int Value, const EScoreValueTypes Type)
{
	switch (Type) {
	case PunchesHit:
		ScoreValues.PunchesHit += Value;
		break;
	case ThrowablesHit:
		ScoreValues.ThrowablesHit += Value;
	case CrownTime:
		ScoreValues.CrownTime += Value;
		break;
	case OutOfMapDeaths:
		ScoreValues.OutOfMapDeaths += Value;
		break;
	case Score: break;
	case BarrelsHit:
		ScoreValues.BarrelsHit += Value;
		break;
	default:;
	}
}

void UScoreSubSystem_B::ResetScoreValues()
{
	ScoreValues = FScoreValues();
}

FScoreValues UScoreSubSystem_B::GetScoreValues() const
{
	return ScoreValues;
}
