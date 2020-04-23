// Fill out your copyright notice in the Description page of Project Settings.

#include "ScoreText_B.h"
#include "Components/TextBlock.h"

void UScoreText_B::DisplayScore(const int Score, const bool bIsMultiplied, const bool bIsAgainstLeader, FLinearColor Color)
{
	if (XOR(bIsMultiplied, bIsAgainstLeader)) 
	{
		ScoreText->SetText(FText::FromString("+" + FString::FormatAsNumber(Score/2) + "x2"));
	}
	else if (bIsMultiplied && bIsAgainstLeader)
	{
		ScoreText->SetText(FText::FromString("+" + FString::FormatAsNumber(Score / 4) + "x4"));
	}
	else
	{
		ScoreText->SetText(FText::FromString("+" + FString::FormatAsNumber(Score)));
	}
	ScoreText->SetColorAndOpacity(Color.CopyWithNewOpacity(1));
	ScoreText->SetVisibility(ESlateVisibility::Visible);
}

void UScoreText_B::HideScore()
{
	ScoreText->SetVisibility(ESlateVisibility::Hidden);
}
