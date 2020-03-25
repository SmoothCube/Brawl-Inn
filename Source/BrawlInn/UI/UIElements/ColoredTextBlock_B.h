// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "System/Structs/ScoreValues.h"
#include "ColoredTextBlock_B.generated.h"

class AGamePlayerController_B;

UCLASS()
class BRAWLINN_API UColoredTextBlock_B : public UTextBlock
{
	GENERATED_BODY()

public:
	void UpdateScore(FScoreValues ScoreValues);
	void SetTextColor(FLinearColor Color);

	UPROPERTY()
	AGamePlayerController_B* OwningPlayer = nullptr;
};
