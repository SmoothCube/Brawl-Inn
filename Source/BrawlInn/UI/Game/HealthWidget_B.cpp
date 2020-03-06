// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthWidget_B.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void UHealthWidget_B::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UHealthWidget_B::Initialize()
{
	const bool s = Super::Initialize();

	UpdateScoreValues(FScoreValues());

	return s;
}

void UHealthWidget_B::UpdateScoreValues(const FScoreValues ScoreValues) const
{
	const FString Text = "Score: " + FString::FromInt(ScoreValues.Score);
	ScoreValueText->SetText(FText::FromString(Text));
}
