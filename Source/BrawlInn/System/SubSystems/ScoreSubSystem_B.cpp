// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreSubSystem_B.h"

#include "BrawlInn.h"

void UScoreSubSystem_B::AddScore(const int Value, const EScoreValueTypes Type)
{
	switch (Type) {
	case Score:
		ScoreValues.Score += Value;
		if (ScoreValues.Score < 0)
			ScoreValues.Score = 0;
		ScoreValues.LastScoreAdded = Value;
		OnScoreChanged.Broadcast(ScoreValues);
		OnScoreChangedNoParam.Broadcast();
		break;
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
