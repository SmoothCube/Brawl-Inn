// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreText_B.generated.h"

class UTextBlock;

UCLASS()
class BRAWLINN_API UScoreText_B : public UUserWidget
{
	GENERATED_BODY()

		UPROPERTY(meta = (BindWidget))
		UTextBlock* ScoreText;
public:
	void DisplayScore(const int Score, const bool bIsMultiplied, const bool bIsAgainstLeader, FLinearColor Color);

	void HideScore();

};
