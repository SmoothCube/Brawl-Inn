// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "BrawlInn.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/ColoredTextBlock_B.h"

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
	ScoreBox->AddChildToVerticalBox(Score);
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
