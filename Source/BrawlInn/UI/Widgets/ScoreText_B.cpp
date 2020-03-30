// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreText_B.h"
#include "Components/TextBlock.h"

void UScoreText_B::DisplayScore(const float Score)
{
	ScoreText->SetText(FText::FromString("+" + FString::FormatAsNumber(Score)));
	ScoreText->SetVisibility(ESlateVisibility::Visible);
}

void UScoreText_B::HideScore()
{
	ScoreText->SetVisibility(ESlateVisibility::Hidden);
}
