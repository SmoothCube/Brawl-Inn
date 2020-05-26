// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "VerticalBox.h"
#include "VerticalBoxSlot.h"
#include "Sound/SoundCue.h"

#include "Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/VictoryGameMode_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "Victory/CharacterVictoryScreen_B.h"
#include "Victory/StatEntry_B.h"

void UVictoryScreenWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	StatBoards.Add(StatTopLeft);
	StatBoards.Add(StatTopRight);
	StatBoards.Add(StatBotLeft);
	StatBoards.Add(StatBotRight);

	for (auto Board : StatBoards)
	{
		Board->StatsBox->ClearChildren();
		for (int i = 0; i < 5; ++i)
		{
			UStatEntry_B* Item = CreateWidget<UStatEntry_B>(Board, BP_StatEntry);
			UVerticalBoxSlot* VSlot = Board->StatsBox->AddChildToVerticalBox(Item);
			VSlot->SetPadding(FMargin(0, 0, 0, 10.f));
		}
	}

	UpdateBackgroundColors();

	AddRandomTitles();

	CountNumbers.AddDefaulted(4);

	PlayStatBlockAnimations();

	DisplayScores(PunchesHit);
	DisplayScores(OutOfMapDeaths);
	DisplayScores(CrownTime);
	DisplayScores(ThrowablesHit);
	DisplayScores(BarrelsHit);
	DisplayScores(Score);

#if WITH_EDITORONLY_DATA
	if (GameInstance->bFakeScoreAtStatScreen)
	{
		for (auto& Count : CountNumbers)
		{
			for (auto& C : Count.Queue)
			{
				C.End = FMath::RandRange(0, 1000);
			}
		}
	}
#endif

}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bCanCount)
	{
		CurrentCountSoundTime += InDeltaTime;
		for (auto& Count : CountNumbers)
		{
			if (Count.Size() == 0)
				continue;

			FormattingOptions.UseGrouping = false;

			if (Count.Top().CurrentTime < Count.Top().Duration)
			{
				Count.Top().CurrentTime += InDeltaTime;
				const int OldValueFloored = Count.Top().ValueFloored;
				Count.Top().ValueFloored = FMath::FloorToInt(UKismetMathLibrary::FInterpEaseInOut(
					Count.Top().Start, Count.Top().End, Count.Top().CurrentTime / Count.Top().Duration, 2));

				if (OldValueFloored != Count.Top().ValueFloored)
				{
					if (CurrentCountSoundTime >= 0.05f)
					{
						CurrentCountSoundTime = 0.f;
						PlaySound(CountSound);
					}
				}

				if (Count.Top().TextBlock)
					Count.Top().TextBlock->SetText(FText::AsNumber(Count.Top().ValueFloored, &FormattingOptions));
			}
			else
			{
				if (Count.Top().TextBlock)
					Count.Top().TextBlock->SetText(FText::AsNumber(Count.Top().End, &FormattingOptions));

				if (Count.Top().End != 0)
					Count.Pop();
			}
		}
	}
}

void UVictoryScreenWidget_B::UpdateBackgroundColors()
{
	if (BackgroundColorsInOrder.Num() != 0 && BannerBotInOrder.Num() != 0 && BannerTopInOrder.Num() != 0)
	{
		const TArray<FPlayerInfo> PlayerInfos = GameInstance->GetPlayerInfos();
		for (int i = 0; i < PlayerInfos.Num(); ++i)
		{
			if (StatBoards[i]->BackgroundMaterial)
			{
				StatBoards[i]->BackgroundMaterial->SetVectorParameterValue("BaseColor", BackgroundColorsInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				StatBoards[i]->BannerBot->Brush.SetResourceObject(BannerBotInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				StatBoards[i]->BannerTop->Brush.SetResourceObject(BannerTopInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				if (!StatBoards[i]->IsVisible())
					StatBoards[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
}

void UVictoryScreenWidget_B::AddRandomTitles()
{
	if (FirstPlaceTitles.Num() == 0 || SecondPlaceTitles.Num() == 0 || ThirdPlaceTitles.Num() == 0 || FourthPlaceTitles.Num() == 0)
		return;

	int RandomIndex = FMath::RandRange(0, FirstPlaceTitles.Num() - 1);
	FString Title = FirstPlaceTitles[RandomIndex];
	StatTopLeft->Title->SetText(FText::FromString(Title));

	RandomIndex = FMath::RandRange(0, SecondPlaceTitles.Num() - 1);
	Title = SecondPlaceTitles[RandomIndex];
	StatTopRight->Title->SetText(FText::FromString(Title));

	RandomIndex = FMath::RandRange(0, ThirdPlaceTitles.Num() - 1);
	Title = ThirdPlaceTitles[RandomIndex];
	StatBotLeft->Title->SetText(FText::FromString(Title));

	RandomIndex = FMath::RandRange(0, FourthPlaceTitles.Num() - 1);
	Title = FourthPlaceTitles[RandomIndex];
	StatBotRight->Title->SetText(FText::FromString(Title));
}

void UVictoryScreenWidget_B::OnStatBlockAnimationsFinished()
{
	bCanCount = true;

	AVictoryGameMode_B* GameMode = Cast<AVictoryGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SetCanContinue(true);
	}
}

void UVictoryScreenWidget_B::DisplayScores(const EScoreValueTypes Type)
{
	const TArray<FPlayerInfo> PlayerInfos = GameInstance->GetPlayerInfos();
	for (int i = 0; i < PlayerInfos.Num(); ++i)
	{
		const FScoreValues ScoreValues = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfos[i].ID)
			->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues();

		if (!StatBoards[i]->IsVisible())
			StatBoards[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);


		switch (Type) {
		case Score:
			AddToCountQueue(0, ScoreValues.Score, StatBoards[i]->ScoreNumber, PlayerInfos[i].ID);
			break;
		case PunchesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(0));
			Stat->Text->SetText(FText::FromString("PUNCHES HIT"));
			AddToCountQueue(0, ScoreValues.PunchesHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case OutOfMapDeaths:
		{

			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(1));
			Stat->Text->SetText(FText::FromString("OUT OF MAP"));
			AddToCountQueue(0, ScoreValues.OutOfMapDeaths, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case CrownTime:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(2));
			Stat->Text->SetText(FText::FromString("CROWN TIME (S)"));
			AddToCountQueue(0, ScoreValues.CrownTime, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case ThrowablesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(3));
			Stat->Text->SetText(FText::FromString("THROWABLES HIT"));
			AddToCountQueue(0, ScoreValues.ThrowablesHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case BarrelsHit:
		{

			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(4));
			Stat->Text->SetText(FText::FromString("BARRELS HIT"));
			AddToCountQueue(0, ScoreValues.BarrelsHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		default:;
		}
	}

}

void UVictoryScreenWidget_B::AddToCountQueue(int Start, int End, UTextBlock* TextBlock, int PlayerControllerID)
{
	if (!TextBlock)
		return;

	CountNumbers[PlayerControllerID].Push(FCountNumber(End, 1, TextBlock));
}
