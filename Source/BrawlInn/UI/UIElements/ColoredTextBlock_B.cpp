// Fill out your copyright notice in the Description page of Project Settings.

#include "ColoredTextBlock_B.h"

void UColoredTextBlock_B::UpdateScore(const FScoreValues ScoreValues)
{
	SetText(FText::FromString(FString::FromInt(ScoreValues.Score)));
}

void UColoredTextBlock_B::SetTextColor(const FLinearColor Color)
{
	SetColorAndOpacity(Color);
}
