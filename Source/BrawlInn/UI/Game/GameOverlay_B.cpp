// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

#include "BrawlInn.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/GameInstance_B.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "UI/UIElements/ColoredTextBlock_B.h"

void UGameOverlay_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<UColoredTextBlock_B*> TextBlocks;

	TextBlocks.Add(P1);
	TextBlocks.Add(P2);
	TextBlocks.Add(P3);
	TextBlocks.Add(P4);

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (GameInstance)
	{
		TArray<ULocalPlayer*> Players = GameInstance->GetLocalPlayers();
		for (ULocalPlayer* PlayerID : Players)
		{
			AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerID->GetControllerId()));
			if (PlayerController)
			{
				PlayerController->SetHealthWidget(TextBlocks[PlayerID->GetControllerId()]);
			}
		}
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
