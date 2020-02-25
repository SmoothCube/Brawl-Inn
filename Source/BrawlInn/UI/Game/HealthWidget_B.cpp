// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthWidget_B.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"

#include "System/GameInstance_B.h"

void UHealthWidget_B::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UHealthWidget_B::Initialize()
{
	bool s = Super::Initialize();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	if (GameInstance->GameIsScoreBased())
	{
		UpdateScoreValues(FScoreValues());
		RespawnCharges->RemoveFromParent();
	}
	else
	{
		ScoreValueText->RemoveFromParent();
		TArray<UWidget*> Children = RespawnCharges->GetAllChildren();

		for (auto& Child : Children)
		{
			UImage* Image = Cast<UImage>(Child);
			if (IsValid(Image))
				Barrels.Push(Image);
		}
	}
	return s;
}

void UHealthWidget_B::UpdateScoreValues(FScoreValues ScoreValues)
{
	if (!GameInstance->GameIsScoreBased())
		return;

	FString Text = "Score: " + FString::FromInt(ScoreValues.Score);
	ScoreValueText->SetText(FText::FromString(Text));
}

void UHealthWidget_B::UpdateHealthAmount(int Amount)
{
	if (GameInstance->GameIsScoreBased())
		return;

	if (IsValid(HealthProgressBar))
		HealthProgressBar->SetPercent((float)Amount / 100.f);
}

void UHealthWidget_B::UpdateRespawnsAmount()
{
	if (GameInstance->GameIsScoreBased())
		return;

	if (Barrels.Num() != 0)
	{
		UImage* Image = Barrels.Pop();
		Image->SetVisibility(ESlateVisibility::Hidden);
	}
}
