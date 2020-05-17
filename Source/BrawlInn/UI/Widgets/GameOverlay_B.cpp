// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/VerticalBox.h"

#include "BrawlInn.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/ColoredTextBlock_B.h"
#include "UMGSequencePlayer.h"

void UGameOverlay_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ScoreArray.Add(P1);
	ScoreArray.Add(P2);
	ScoreArray.Add(P3);
	ScoreArray.Add(P4);

	GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(GameMode));

	GameMode->OnAnyScoreChange().AddUObject(this, &UGameOverlay_B::UpdateScoreList);

	for (auto PlayerController : GameMode->GetPlayerControllers())
	{
		AGamePlayerController_B* GamePlayerController = Cast<AGamePlayerController_B>(PlayerController);
		if (GamePlayerController)
		{
			GamePlayerController->SetScoreTextBlock(ScoreArray[UGameplayStatics::GetPlayerControllerID(GamePlayerController)]);
			ScoreArray[UGameplayStatics::GetPlayerControllerID(GamePlayerController)]->OwningPlayer = GamePlayerController;
		}
	}

	FirstTextLine->SetVisibility(ESlateVisibility::Hidden);
	SecondTextLine->SetVisibility(ESlateVisibility::Hidden);

	ShowScoreBoardAndClock();
}

void UGameOverlay_B::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Text Display
	if (bShouldDisplay)
	{
		if (CurrentDisplayTime < DisplayTime)
		{
			CurrentDisplayTime += InDeltaTime;
		}
		else
		{
			bShouldDisplay = false;
			CurrentDisplayTime = 0.f;
			PlayFadeTextAnimation(false);
		}
	}
}

void UGameOverlay_B::DisplayText(const FString FirstLine, const FString SecondLine, const float TimeToDisplay)
{
	FirstTextLine->SetText(FText::FromString(FirstLine));
	SecondTextLine->SetText(FText::FromString(SecondLine));

	DisplayTime = TimeToDisplay;
	CurrentDisplayTime = 0.f;
	FirstTextLine->SetVisibility(ESlateVisibility::HitTestInvisible);
	SecondTextLine->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayFadeTextAnimation(true);

}

void UGameOverlay_B::OnFadeTextFinished(const bool bIsPlayingForward)
{
	if (bIsPlayingForward)
	{
		bShouldDisplay = true;
	}
	else
	{
		bShouldDisplay = false;
		FirstTextLine->SetVisibility(ESlateVisibility::Hidden);
		SecondTextLine->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameOverlay_B::UpdateScoreList()
{
	ScoreArray.Sort([&](const UColoredTextBlock_B& Left, const UColoredTextBlock_B& Right)
		{
			return Left.OwningPlayer->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score >
				Right.OwningPlayer->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
		});

	ScoreBox->ClearChildren();

	for (auto Score : ScoreArray)
	{
		UVerticalBoxSlot* VSlot = ScoreBox->AddChildToVerticalBox(Score);
		//FMargin Margin;
		//Margin.Bottom = 15.f;
		//VSlot->SetPadding(Margin);
	}
}

void UGameOverlay_B::UpdateTimerText(const int TimeRemaining)
{
	if (TimeRemaining < 0 || !IsValid(TimeText))
		return;

	const int Minutes = TimeRemaining / 60;
	const int Seconds = TimeRemaining - 60 * Minutes;

	FNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = 2;

	const FText Display = FText::Join(FText::FromString(":"), FText::AsNumber(Minutes, &Options), FText::AsNumber(Seconds, &Options));
	TimeText->SetText(Display);
}
